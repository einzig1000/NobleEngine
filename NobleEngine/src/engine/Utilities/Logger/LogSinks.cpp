#include "LogSinks.h"
#include <externals/MagicEnum/magic_enum.hpp>
#include <algorithm>
#include <filesystem>
#include <format>

namespace
{
    // フルパスからファイル名だけを取り出す
    const char* GetFileNameOnly(const char* path)
    {
        const char* result = path;
        for (const char* p = path; *p != '\0'; ++p)
        {
            if (*p == '\\' || *p == '/') result = p + 1;
        }
        return result;
    }

    // UTF-8 の文字列をデバッガの出力ウィンドウに出す
    void OutputUtf8ToDebugger(const std::string& text)
    {
        const int textLength = static_cast<int>(text.size());
        const int wideLength = MultiByteToWideChar(CP_UTF8, 0, text.data(), textLength, nullptr, 0);
        if (wideLength > 0)
        {
            std::wstring wide(static_cast<size_t>(wideLength), L'\0');
            MultiByteToWideChar(CP_UTF8, 0, text.data(), textLength, wide.data(), wideLength);
            OutputDebugStringW(wide.c_str());
        }
        else
        {
            OutputDebugStringA("[Logger] UTF-8 to UTF-16 conversion failed. Raw bytes:\n");
            OutputDebugStringA(text.c_str());
        }
    }
}

namespace Logger
{
#pragma region DebuggerSink

    void DebuggerSink::Write(const LogEntry& entry)
    {
        const std::string_view levelName = magic_enum::enum_name(entry.level);
        if (entry.level >= LogLevel::Warning)
        {
            OutputUtf8ToDebugger(std::format("{}({}): [{}] {}\n", entry.file, entry.line, levelName, entry.message));
        }
        else
        {
            OutputUtf8ToDebugger(std::format("[{:<7}] {}\n", levelName, entry.message));
        }
    }

#pragma endregion

#pragma region FileSink

    FileSink::FileSink(uint32_t acceptLevels, std::wstring directory, size_t maxFiles)
        : ILogSink(acceptLevels), directory_(std::move(directory)), maxFiles_(maxFiles)
    {}

    FileSink::~FileSink()
    {
        if (file_ != INVALID_HANDLE_VALUE)
        {
            CloseHandle(file_);
        }
    }

    void FileSink::Write(const LogEntry& entry)
    {
        if (!openTried_) Open();
        if (file_ == INVALID_HANDLE_VALUE) return;

        std::string line = std::format("[{}][{:<7}] {}", entry.time, magic_enum::enum_name(entry.level), entry.message);
        if (entry.level >= LogLevel::Warning)
        {
            line += std::format("  ({}:{})", GetFileNameOnly(entry.file), entry.line);
        }
        line += "\r\n";

        // 1行ごとに OS へ渡すので、途中で落ちてもそこまでは残る
        DWORD written = 0;
        WriteFile(file_, line.data(), static_cast<DWORD>(line.size()), &written, nullptr);
    }

    void FileSink::Open()
    {
        openTried_ = true;

        std::error_code ec;
        std::filesystem::create_directories(directory_, ec);
        if (ec) return;

        // 古い順に消して、今回の分を足しても maxFiles_ 本になるようにする
        // 名前が「年-月-日_時-分-秒」なので、名前順がそのまま日時順になる
        std::vector<std::filesystem::path> oldFiles;
        for (std::filesystem::directory_iterator it(directory_, ec), end; !ec && it != end; it.increment(ec))
        {
            if (it->path().extension() == L".log")
            {
                oldFiles.push_back(it->path());
            }
        }
        std::sort(oldFiles.begin(), oldFiles.end());
        while (!oldFiles.empty() && oldFiles.size() >= maxFiles_)
        {
            std::filesystem::remove(oldFiles.front(), ec);
            oldFiles.erase(oldFiles.begin());
        }

        // 新しいファイルを作る。同じ秒に2つ起動したときは _2, _3 ... を付ける
        SYSTEMTIME now{};
        GetLocalTime(&now);
        const std::wstring baseName = std::format(L"{:04}-{:02}-{:02}_{:02}-{:02}-{:02}",
            now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond);

        for (int number = 1; number <= 9; ++number)
        {
            const std::wstring suffix = (number == 1) ? std::wstring() : std::format(L"_{}", number);
            const std::filesystem::path path = std::filesystem::path(directory_) / (baseName + suffix + L".log");

            HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (file != INVALID_HANDLE_VALUE)
            {
                file_ = file;
                filePath_ = std::filesystem::absolute(path, ec).wstring();

                const std::string header = std::format("NobleEngine log  {:04}-{:02}-{:02} {:02}:{:02}:{:02}\r\n",
                    now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond);
                DWORD written = 0;
                WriteFile(file_, header.data(), static_cast<DWORD>(header.size()), &written, nullptr);
                return;
            }
            if (GetLastError() != ERROR_FILE_EXISTS) return;
        }
    }

#pragma endregion

#pragma region HistorySink

    HistorySink::HistorySink(uint32_t acceptLevels, size_t capacity)
        : ILogSink(acceptLevels), capacity_(capacity)
    {}

    void HistorySink::Write(const LogEntry& entry)
    {
        entries_.push_back(entry);
        if (entries_.size() > capacity_)
        {
            entries_.pop_front();
        }
    }

    size_t HistorySink::CopySince(uint64_t lastSerial, std::vector<LogEntry>& out) const
    {
        // 通し番号の順に並んでいるので、二分探索で「まだ渡していない最初の1件」を探す
        auto first = std::upper_bound(entries_.begin(), entries_.end(), lastSerial,
            [](uint64_t serial, const LogEntry& entry) { return serial < entry.serial; });
        const size_t count = static_cast<size_t>(std::distance(first, entries_.end()));
        out.insert(out.end(), first, entries_.end());
        return count;
    }

#pragma endregion
}
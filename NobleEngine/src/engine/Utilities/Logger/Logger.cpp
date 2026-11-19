#include "Logger.h"
#include "LogSinks.h"
#include <externals/MagicEnum/magic_enum.hpp>
#include <cstdarg>
#include <cstdio>
#include <format>
#include <sstream>

namespace
{
    using namespace Logger;

    static_assert(magic_enum::enum_count<LogLevel>() == kLogLevelCount, "LogTypes.h の kLogLevelCount を LogLevel の数に合わせてください");

    // Logs フォルダとそこに残すファイルの本数（今回の分を含む）
    constexpr const wchar_t* kLogDirectory = L"Logs";
    constexpr size_t kMaxLogFiles = 10;

    // D3D12_RESOURCE_STATES を文字列に変換する関数
    // （値が 127 を超えるビットの組み合わせなので magic_enum では引けない）
    std::string ResourceStateToString(D3D12_RESOURCE_STATES state)
    {
        switch (state)
        {
        case D3D12_RESOURCE_STATE_COMMON: return "COMMON";
        case D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER: return "VERTEX_AND_CONSTANT_BUFFER";
        case D3D12_RESOURCE_STATE_INDEX_BUFFER: return "INDEX_BUFFER";
        case D3D12_RESOURCE_STATE_RENDER_TARGET: return "RENDER_TARGET";
        case D3D12_RESOURCE_STATE_UNORDERED_ACCESS: return "UNORDERED_ACCESS";
        case D3D12_RESOURCE_STATE_DEPTH_WRITE: return "DEPTH_WRITE";
        case D3D12_RESOURCE_STATE_DEPTH_READ: return "DEPTH_READ";
        case D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE: return "NON_PIXEL_SHADER_RESOURCE";
        case D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE: return "PIXEL_SHADER_RESOURCE";
        case D3D12_RESOURCE_STATE_STREAM_OUT: return "STREAM_OUT";
        case D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT: return "INDIRECT_ARGUMENT";
        case D3D12_RESOURCE_STATE_COPY_DEST: return "COPY_DEST";
        case D3D12_RESOURCE_STATE_COPY_SOURCE: return "COPY_SOURCE";
        case D3D12_RESOURCE_STATE_RESOLVE_DEST: return "RESOLVE_DEST";
        case D3D12_RESOURCE_STATE_RESOLVE_SOURCE: return "RESOLVE_SOURCE";
        case D3D12_RESOURCE_STATE_VIDEO_DECODE_READ: return "VIDEO_DECODE_READ";
        case D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE: return "VIDEO_DECODE_WRITE";
        case D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ: return "VIDEO_PROCESS_READ";
        case D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE: return "VIDEO_PROCESS_WRITE";
        case D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ: return "VIDEO_ENCODE_READ";
        case D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE: return "VIDEO_ENCODE_WRITE";
        default: return "UNKNOWN_STATE";
        }
    }

    // ロガーの状態
    struct LoggerState
    {
        std::vector<std::unique_ptr<ILogSink>> sinks; // 全部の出力先。ログはこの順に配られる

        // 最初から持っている出力先（中身は sinks が持っている。ここは探す手間を省くための近道）
        DebuggerSink* debugger = nullptr;
        FileSink* file = nullptr;
        HistorySink* history = nullptr;

        uint64_t nextSerial = 1;

        LoggerState()
        {
            auto debuggerSink = std::make_unique<DebuggerSink>(kAllLogLevels);
            auto fileSink = std::make_unique<FileSink>(ToMask({ LogLevel::Info, LogLevel::Warning, LogLevel::Error }), kLogDirectory, kMaxLogFiles);
            auto historySink = std::make_unique<HistorySink>(kAllLogLevels, kHistoryCapacity);

            debugger = debuggerSink.get();
            file = fileSink.get();
            history = historySink.get();

            sinks.push_back(std::move(debuggerSink));
            sinks.push_back(std::move(fileSink));
            sinks.push_back(std::move(historySink));
        }

        ILogSink* GetBuiltInSink(LogOutput output) const
        {
            switch (output)
            {
            case LogOutput::Debugger: return debugger;
            case LogOutput::File:     return file;
            case LogOutput::History:  return history;
            default:                  return nullptr;
            }
        }
    };

    // わざと解放しない。他のクラスのデストラクタ（終了処理の最後の方）から呼ばれても安全に書けるようにするため
    // ファイルのハンドルはプロセス終了時に OS が閉じる
    LoggerState& State()
    {
        static LoggerState* state = new LoggerState();
        return *state;
    }

    // 別スレッドから同時に呼ばれても行が混ざらないようにする鍵（デストラクタが無い型なので終了時も安全）
    SRWLOCK gLock = SRWLOCK_INIT;
}


namespace Logger
{
    // outputにはどのレベルのログを送るか指定する
    void SetAcceptLevels(LogOutput output, std::initializer_list<LogLevel> levels)
    {
        AcquireSRWLockShared(&gLock);
        if (ILogSink* sink = State().GetBuiltInSink(output))
        {
            sink->SetAcceptLevels(ToMask(levels));
        }
        ReleaseSRWLockShared(&gLock);
    }

    void AddSink(std::unique_ptr<ILogSink> sink)
    {
        if (!sink) return;
        AcquireSRWLockExclusive(&gLock);
        State().sinks.push_back(std::move(sink));
        ReleaseSRWLockExclusive(&gLock);
    }

    /// 履歴のうちIDが最後尾より新しいものをoutの後ろに足す。戻り値は足した件数
    /// 履歴は最新の kHistoryCapacity 件だけ残る
    size_t CopyHistorySince(uint64_t lastSerial, std::vector<LogEntry>& out)
    {
        AcquireSRWLockShared(&gLock);
        const size_t count = State().history->CopySince(lastSerial, out);
        ReleaseSRWLockShared(&gLock);
        return count;
    }

    std::wstring GetLogFilePath()
    {
        AcquireSRWLockShared(&gLock);
        std::wstring path = State().file->GetFilePath();
        ReleaseSRWLockShared(&gLock);
        return path;
    }

    std::string HrToString(HRESULT hr)
    {
        const char* name = "UNKNOWN_HRESULT";
        switch (hr)
        {
        case S_OK: name = "S_OK"; break;
        case E_FAIL: name = "E_FAIL"; break;
        case E_INVALIDARG: name = "E_INVALIDARG"; break;
        case E_OUTOFMEMORY: name = "E_OUTOFMEMORY"; break;
        case E_NOTIMPL: name = "E_NOTIMPL"; break;
        case E_NOINTERFACE: name = "E_NOINTERFACE"; break;
        case E_ACCESSDENIED: name = "E_ACCESSDENIED"; break;

        case DXGI_ERROR_DEVICE_REMOVED: name = "DXGI_ERROR_DEVICE_REMOVED"; break;
        case DXGI_ERROR_DEVICE_HUNG: name = "DXGI_ERROR_DEVICE_HUNG"; break;
        case DXGI_ERROR_DEVICE_RESET: name = "DXGI_ERROR_DEVICE_RESET"; break;
        case DXGI_ERROR_DRIVER_INTERNAL_ERROR: name = "DXGI_ERROR_DRIVER_INTERNAL_ERROR"; break;
        case DXGI_ERROR_INVALID_CALL: name = "DXGI_ERROR_INVALID_CALL"; break;
        case DXGI_ERROR_UNSUPPORTED: name = "DXGI_ERROR_UNSUPPORTED"; break;
        case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE: name = "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE"; break;

        case D3D12_ERROR_ADAPTER_NOT_FOUND: name = "D3D12_ERROR_ADAPTER_NOT_FOUND"; break;
        case D3D12_ERROR_DRIVER_VERSION_MISMATCH: name = "D3D12_ERROR_DRIVER_VERSION_MISMATCH"; break;
        default: break;
        }
        return std::format("{} (0x{:08X})", name, static_cast<uint32_t>(hr));
    }

    // barrier.Transitionの状態をログに出力する関数
    void Debug(const D3D12_RESOURCE_BARRIER& barrier)
    {
        if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
        {
            Debug("Barrier Transition - StateBefore: %s, StateAfter: %s",
                ResourceStateToString(barrier.Transition.StateBefore),
                ResourceStateToString(barrier.Transition.StateAfter));
        }
        else
        {
            Debug("Barrier is not of type TRANSITION.");
        }
    }

    // RootSignatureDescの状態をログに出力する関数
    void Debug(const D3D12_ROOT_SIGNATURE_DESC& desc)
    {
        std::ostringstream oss;
        oss << "[Root Signature]\n"
            << "NumParameters: " << desc.NumParameters << "\n"
            << "NumStaticSamplers: " << desc.NumStaticSamplers << "\n"
            << "Flags: " << desc.Flags << "\n";

        Debug("%s", oss.str());

        for (UINT i = 0; i < desc.NumParameters; ++i)
        {
            const auto& param = desc.pParameters[i];
            oss.str(""); // バッファをクリア
            oss.clear(); // 状態をリセット
            oss << "[Root Parameter " << i << "]\n"
                << "Type: " << param.ParameterType << "\n"
                << "ShaderVisibility: " << param.ShaderVisibility << "\n";
            Debug("%s", oss.str());

            if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV ||
                param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV ||
                param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_UAV)
            {
                oss.str("");
                oss.clear();
                oss << "ShaderRegister: " << param.Descriptor.ShaderRegister << "\n"
                    << "RegisterSpace: " << param.Descriptor.RegisterSpace << "\n";
                Debug("%s", oss.str());
            }
            else if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
            {
                oss.str("");
                oss.clear();
                oss << "NumDescriptorRanges: " << param.DescriptorTable.NumDescriptorRanges << "\n";
                Debug("%s", oss.str());
                for (UINT j = 0; j < param.DescriptorTable.NumDescriptorRanges; ++j)
                {
                    const auto& range = param.DescriptorTable.pDescriptorRanges[j];
                    oss.str("");
                    oss.clear();
                    oss << "  [Descriptor Range " << j << "]\n"
                        << "  RangeType: " << range.RangeType << "\n"
                        << "  BaseShaderRegister: " << range.BaseShaderRegister << "\n"
                        << "  NumDescriptors: " << range.NumDescriptors << "\n"
                        << "  RegisterSpace: " << range.RegisterSpace << "\n";
                    Debug("%s", oss.str());
                }
            }
        }

        for (UINT i = 0; i < desc.NumStaticSamplers; ++i)
        {
            const auto& sampler = desc.pStaticSamplers[i];
            oss.str("");
            oss.clear();
            oss << "[Static Sampler " << i << "]\n"
                << "ShaderRegister: " << sampler.ShaderRegister << "\n"
                << "Filter: " << sampler.Filter << "\n"
                << "AddressU: " << sampler.AddressU << "\n"
                << "AddressV: " << sampler.AddressV << "\n"
                << "AddressW: " << sampler.AddressW << "\n"
                << "ShaderVisibility: " << sampler.ShaderVisibility << "\n";
            Debug("%s", oss.str());
        }
    }

    namespace Detail
    {
        bool IsEnabled(LogLevel level)
        {
            AcquireSRWLockShared(&gLock);
            bool enabled = false;
            for (const auto& sink : State().sinks)
            {
                if (sink->Accepts(level))
                {
                    enabled = true;
                    break;
                }
            }
            ReleaseSRWLockShared(&gLock);
            return enabled;
        }

        // printf 形式の本体。長さを先に測ってから、ぴったりの大きさで書く
        void WriteFormatted(LogLevel level, const std::source_location& location, const char* format, ...)
        {
            // 1. 整形する（鍵の外でやる）
            va_list args;
            va_start(args, format);

            va_list argsForLength;               // va_list は1回しか使えないので、長さを測る用に複製する
            va_copy(argsForLength, args);
            const int length = _vscprintf(format, argsForLength);
            va_end(argsForLength);

            std::string message;
            if (length < 0)
            {
                message = std::string("[Logger] 書式の展開に失敗しました: ") + format;
            }
            else
            {
                message.resize(static_cast<size_t>(length));
                vsnprintf(message.data(), static_cast<size_t>(length) + 1, format, args); // +1 は終端の \0 の分
            }
            va_end(args);

            // 末尾の改行は出力先の側で付けるので取る
            while (!message.empty() && (message.back() == '\n' || message.back() == '\r'))
            {
                message.pop_back();
            }

            SYSTEMTIME now{};
            GetLocalTime(&now);

            LogEntry entry;
            entry.level = level;
            entry.time = std::format("{:02}:{:02}:{:02}.{:03}", now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);
            entry.message = std::move(message);
            entry.file = location.file_name();
            entry.line = location.line();

            // 2. 受け取る出力先に配る
            AcquireSRWLockExclusive(&gLock);
            LoggerState& state = State();
            entry.serial = state.nextSerial++;
            for (const auto& sink : state.sinks)
            {
                if (sink->Accepts(level))
                {
                    sink->Write(entry);
                }
            }
            ReleaseSRWLockExclusive(&gLock);
        }
    }
}
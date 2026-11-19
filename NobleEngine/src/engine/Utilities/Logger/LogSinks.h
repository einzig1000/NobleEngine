#pragma once
#include <Utilities/Logger/ILogSink.h>
#include <Windows.h>
#include <deque>
#include <string>
#include <vector>

// 出力先ごとのクラス
namespace Logger
{
	/// <summary>
	/// Visual Studio の出力ウィンドウに出す
	/// Warning 以上は「ファイル(行): 」で始めるので、ダブルクリックでその行へ飛べる
	/// </summary>
	class DebuggerSink : public ILogSink
	{
	public:
		using ILogSink::ILogSink;
		void Write(const LogEntry& entry) override;
	};

	/// <summary>
	/// Logs/年-月-日_時-分-秒.log に書く。起動ごとに1本作り、古いものは消して最新 maxFiles 本を残す
	/// ファイルは最初の1件が来たときに作る
	/// </summary>
	class FileSink : public ILogSink
	{
	public:
		FileSink(uint32_t acceptLevels, std::wstring directory, size_t maxFiles);
		~FileSink() override;
		void Write(const LogEntry& entry) override;

		/// <summary>
		/// 書いているファイルのフルパス。まだ作っていない・作れなかったときは空
		/// </summary>
		const std::wstring& GetFilePath() const { return filePath_; }

	private:
		// 今回の起動用のファイルを作る。失敗しても2回目は試さない
		void Open();

		std::wstring directory_;
		size_t maxFiles_ = 10;
		bool openTried_ = false;
		HANDLE file_ = INVALID_HANDLE_VALUE;
		std::wstring filePath_;
	};

	/// <summary>
	/// メモリに最新 capacity 件を残す。ImGui のログ窓が CopySince で読む
	/// </summary>
	class HistorySink : public ILogSink
	{
	public:
		HistorySink(uint32_t acceptLevels, size_t capacity);
		void Write(const LogEntry& entry) override;

		/// <summary>
		/// serial が lastSerial より新しいものを out の後ろに足す。戻り値は足した件数
		/// </summary>
		size_t CopySince(uint64_t lastSerial, std::vector<LogEntry>& out) const;

	private:
		std::deque<LogEntry> entries_;
		size_t capacity_ = 0;
	};
}
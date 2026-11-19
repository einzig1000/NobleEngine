#pragma once
#include <Utilities/Logger/LogTypes.h>
#include <atomic>

namespace Logger
{
	/// <summary>
	/// ログの出力先1つ分。VS の出力・ファイル・履歴などがこれを継承する
	/// Write は Logger の鍵の中で呼ばれるので、出力先の側で鍵をかける必要はない
	/// </summary>
	class ILogSink
	{
	public:
		explicit ILogSink(uint32_t acceptLevels = kAllLogLevels) : acceptLevels_(acceptLevels) {}
		virtual ~ILogSink() = default;

		ILogSink(const ILogSink&) = delete;
		ILogSink& operator=(const ILogSink&) = delete;

		// 1件書く
		virtual void Write(const LogEntry& entry) = 0;

		// この重要度を受け取るか
		bool Accepts(LogLevel level) const { return (acceptLevels_.load(std::memory_order_relaxed) & ToMask(level)) != 0; }

		// 受け取る重要度を決める。例: SetAcceptLevels(ToMask({ LogLevel::Info, LogLevel::Error }))
		void SetAcceptLevels(uint32_t levels) { acceptLevels_.store(levels, std::memory_order_relaxed); }
		uint32_t GetAcceptLevels() const { return acceptLevels_.load(std::memory_order_relaxed); }

	private:
		// 別スレッドから設定を変えても壊れないようにatomic
		std::atomic<uint32_t> acceptLevels_;
	};
}

#pragma once
#include <cstdint>
#include <initializer_list>
#include <string>

namespace Logger
{
	// ログの重要度
	enum class LogLevel : uint8_t
	{
		// 開発中だけ見たい細かい情報
		Debug,
		// 起動の流れなど、releaseでも残したい情報
		Info,
		// おかしいが続けられる
		Warning,
		// 失敗した
		Error,
	};

	inline constexpr size_t kLogLevelCount = 4;
	
	constexpr uint32_t ToMask(LogLevel level)
	{
		return uint32_t{ 1 } << static_cast<uint32_t>(level);
	}

	constexpr uint32_t ToMask(std::initializer_list<LogLevel> levels)
	{
		uint32_t mask = 0;
		for (LogLevel level : levels) mask |= ToMask(level);
		return mask;
	}

	inline constexpr uint32_t kAllLogLevels = (uint32_t{ 1 } << kLogLevelCount) - 1;

	// ログ一件分の情報
	struct LogEntry
	{
		// 通し番号
		uint64_t serial = 0;
		// 重要度
		LogLevel level = LogLevel::Debug;
		// 時間
		std::string time;
		// メッセージ
		std::string message;
		// 呼び出し元のファイル名
		const char* file = "";
		// 呼び出し元の行
		uint32_t line = 0;
	};
}
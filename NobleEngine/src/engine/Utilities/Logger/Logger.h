//#pragma once
//#include <d3d12.h>
//#include <Windows.h>
//
//
//namespace Logger
//{
//	/// <summary>
//	/// デバッグ用のログを出力する関数 (D3D12_RESOURCE_BARRIER の状態を含む)
//	/// </summary>
//	/// <param name="message">出力するメッセージ</param>
//	/// <param name="barrier">ログに出力するリソースバリア</param>
//	void Log(const D3D12_RESOURCE_BARRIER& barrier);
//
//	/// <summary>
//	/// デバッグ用のログを出力する関数 (D3D12_ROOT_SIGNATURE_DESC の状態を含む)
//	/// </summary>
//	/// <param name="desc">ログに出力するルートシグネチャの記述子</param>
//	void Log(const D3D12_ROOT_SIGNATURE_DESC& desc);
//
//	/// <summary>
//	/// デバッグ用のログを出力する関数 (HRESULT の状態を含む)
//	/// </summary>
//	/// <param name="hr">ログに出力するHRESULT</param>
//	void Log(const HRESULT& hr);
//
//
//
//	/// <summary>
//	/// printfみたいに使えるログ関数
//	/// </summary>
//	void Log(const char* format, ...);
//}


#pragma once
#include <Utilities/Logger/LogTypes.h>
#include <d3d12.h>
#include <Windows.h>
#include <memory>
#include <source_location>
#include <string>
#include <type_traits>
#include <vector>

namespace Logger
{
	class ILogSink;

	// 出力先enum
	enum class LogOutput : uint8_t
	{
		Debugger, // VSの出力ウィンドウ
		File,     // Logsフォルダのファイル
		History,  // メモリ上の履歴
	};

	// そのソースがどこにあるかを保存する構造体
	struct FormatWithLocation
	{
		const char* format;
		std::source_location location;

		FormatWithLocation(const char* text, std::source_location caller = std::source_location::current())
			: format(text), location(caller)
		{}
	};

	// outputにはどのレベルのログを送るか指定する
	void SetAcceptLevels(LogOutput output, std::initializer_list<LogLevel> levels);

	// 出力先を足す（ゲーム画面に出す、など）。受け取る重要度は sink 側で決めておく
	void AddSink(std::unique_ptr<ILogSink> sink);

	// 履歴のうちIDが最後尾より新しいものをoutの後ろに足す。戻り値は足した件数
	// 履歴は最新の kHistoryCapacity 件だけ残る
	size_t CopyHistorySince(uint64_t lastSerial, std::vector<LogEntry>& out);
	inline constexpr size_t kHistoryCapacity = 4096;

	// 今回の起動で書いているログファイルのパス
	std::wstring GetLogFilePath();


	// HRESULTをエラー文字列にする
	std::string HrToString(HRESULT hr);
	// D3D12 の構造体の中身を Debug で出す
	void Debug(const D3D12_RESOURCE_BARRIER& barrier);
	void Debug(const D3D12_ROOT_SIGNATURE_DESC& desc);

	namespace Detail
	{
		// printf 形式で整形して出力先に配る本体。直接は呼ばず、下の Debug / Info / Warning / Error を使う
		void WriteFormatted(LogLevel level, const std::source_location& location, const char* format, ...);

		// その重要度を受け取る出力先が1つでもあるか
		bool IsEnabled(LogLevel level);

		// printfの...に渡せる形に直す。std::string は c_str() に変える
		// それ以外で渡せない型（Vector3 や std::string_view など）はコンパイルエラーにする
		template<class T>
		auto ToPrintfArg(const T& value)
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				return value.c_str();
			}
			else
			{
				static_assert(std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_pointer_v<T> || std::is_array_v<T>,
					"Log に渡せるのは 数値・列挙型・ポインタ・文字列(const char* / std::string) だけです");
				return value;
			}
		}

		template<class... Args>
		void Write(LogLevel level, const FormatWithLocation& format, const Args&... args)
		{
			if (!IsEnabled(level)) return;
			WriteFormatted(level, format.location, format.format, ToPrintfArg(args)...);
		}
	}

	/// <summary>
	/// printf みたいに使えるログ関数。std::string もそのまま %s に渡せる
	/// </summary>
	template<class... Args>
	void Debug(FormatWithLocation format, const Args&... args) { Detail::Write(LogLevel::Debug, format, args...); }

	template<class... Args>
	void Info(FormatWithLocation format, const Args&... args) { Detail::Write(LogLevel::Info, format, args...); }

	template<class... Args>
	void Warning(FormatWithLocation format, const Args&... args) { Detail::Write(LogLevel::Warning, format, args...); }

	template<class... Args>
	void Error(FormatWithLocation format, const Args&... args) { Detail::Write(LogLevel::Error, format, args...); }
	
	/// <summary>
	/// 移行用。中身は Debug と同じ。「Logger::Log(」で検索して、Debug / Info / Warning / Error に置き換えていく
	///	</summary>
	template<class... Args>
	void Log(FormatWithLocation format, const Args&... args) { Detail::Write(LogLevel::Debug, format, args...); }
}
#pragma once
#include <string>

namespace StringConverter
{
	/// <summary>
	/// UTF-8 文字列をワイド文字列 (UTF-16) に変換する関数
	/// </summary>
	/// <param name="str">変換する UTF-8 文字列</param>
	/// <returns>変換されたワイド文字列</returns>
	std::wstring Convert(const std::string& str);

	/// <summary>
	/// ワイド文字列 (UTF-16) を UTF-8 文字列に変換する関数
	/// </summary>
	/// <param name="str">変換するワイド文字列</param>
	/// <returns>変換された UTF-8 文字列</returns>
	std::string Convert(const std::wstring& str);
}
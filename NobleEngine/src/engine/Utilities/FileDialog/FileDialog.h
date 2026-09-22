#pragma once
#include <Windows.h>
#include <string>
#include <vector>

namespace FileDialog
{
	// フィルタ1つ分(表示名と拡張子パターン)
	struct Filter
	{
		std::wstring name; // 例: L"Model Files"
		std::wstring spec; // 例: L"*.obj"
	};

	// ファイルを開くダイアログを表示する。キャンセル時は空文字を返す。
	// 戻り値は実行ディレクトリ(assetsフォルダがある場所)からの相対パスに変換される。
	// defaultFolderは実行ディレクトリからの相対パスで、ダイアログが最初に開くフォルダを指定する
	std::string OpenFile(HWND owner, const std::wstring& title, const std::vector<Filter>& filters, const std::wstring& defaultFolder = L"assets");
}
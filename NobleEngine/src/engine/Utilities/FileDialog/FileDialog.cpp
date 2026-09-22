#include "FileDialog.h"
#include <Utilities/Converter/StringConverter/StringConverter.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <thread>
#include <filesystem>

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Shell32.lib")

namespace
{
	std::string ShowOpenFileDialogOnSTAThread(const std::wstring& title, const std::vector<FileDialog::Filter>& filters, const std::wstring& defaultFolder)
	{
		std::string result;

		HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(hr)) return result;

		IFileOpenDialog* dialog = nullptr;
		hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog));
		if (SUCCEEDED(hr))
		{
			dialog->SetTitle(title.c_str());

			std::vector<COMDLG_FILTERSPEC> specs;
			specs.reserve(filters.size());
			for (const auto& filter : filters)
			{
				specs.push_back({ filter.name.c_str(), filter.spec.c_str() });
			}
			if (!specs.empty())
			{
				dialog->SetFileTypes(static_cast<UINT>(specs.size()), specs.data());
			}

			// 初期フォルダを設定(実行ディレクトリ基準の相対パス→絶対パスに変換して渡す)
			if (!defaultFolder.empty())
			{
				std::wstring absoluteDefaultFolder = std::filesystem::absolute(defaultFolder).wstring();
				IShellItem* folderItem = nullptr;
				if (SUCCEEDED(SHCreateItemFromParsingName(absoluteDefaultFolder.c_str(), nullptr, IID_PPV_ARGS(&folderItem))))
				{
					dialog->SetFolder(folderItem);
					folderItem->Release();
				}
			}

			// owner(メインスレッドが持つゲームウィンドウ)を渡すと、ダイアログ生成時にそのウィンドウへ
			// 同期メッセージを送ろうとすることがある。呼び出し元スレッドはjoin()で待つだけで
			// メッセージポンプを回していないため、そこで送受信がかみ合わずデッドロックする
			// (dui70.dll/duser.dllロード直後でハングする実測と一致)。ownerを渡さず表示することで回避する。
			hr = dialog->Show(nullptr);
			if (SUCCEEDED(hr))
			{
				IShellItem* item = nullptr;
				hr = dialog->GetResult(&item);
				if (SUCCEEDED(hr))
				{
					PWSTR path = nullptr;
					hr = item->GetDisplayName(SIGDN_FILESYSPATH, &path);
					if (SUCCEEDED(hr))
					{
						result = StringConverter::Convert(std::wstring(path));
						CoTaskMemFree(path);
					}
					item->Release();
				}
			}
			dialog->Release();
		}

		CoUninitialize();
		return result;
	}
}

std::string FileDialog::OpenFile(HWND owner, const std::wstring& title, const std::vector<Filter>& filters, const std::wstring& defaultFolder)
{
	(void)owner;

	std::string result;
	std::thread worker([&]()
		{
			result = ShowOpenFileDialogOnSTAThread(title, filters, defaultFolder);
		});
	worker.join();

	if (result.empty())
	{
		return result;
	}

	// エンジンはassetsフォルダがある実行ディレクトリからの相対パスでアセットを管理しているため、
	// 選んだファイルも他のモデル/テクスチャと同じ形式に揃える
	std::error_code ec;
	std::filesystem::path relativePath = std::filesystem::relative(result, std::filesystem::current_path(), ec);
	if (!ec && !relativePath.empty())
	{
		result = relativePath.generic_string(); // 区切り文字を'/'に統一
	}

	return result;
}
#pragma once
#include <Windows.h>        // ウィンドウ関連のAPI
#include <string>           // std::wstring 用
#include <mmsystem.h>

class MouseController;

/// <summary>
/// ウィンドウ管理クラス
/// </summary>
class WindowManager {
public:
    WindowManager(int width, int height, const std::wstring& title);
    ~WindowManager();

    void AttachMouseController(MouseController* mc);

	// ウィンドウハンドル取得
    HWND GetHwnd() const { return hwnd_; }

	// フルスクリーン制御
    void SetFullscreen(bool enable);
    void ToggleFullscreen();
    bool IsFullscreen() const { return isFullscreen_; }

    void Quit();

    // ウィンドウサイズ
    static uint32_t winWidth_;
    static uint32_t winHeight_;

	// アクティブフラグ(ウィンドウが最小化されている時とかはfalse)
	bool isActive_ = true;

private:
    HWND hwnd_;
    WNDCLASS wc_;


    // フルスクリーン制御
    bool isFullscreen_ = false;
    DWORD windowedStyle_ = 0;
    DWORD windowedExStyle_ = 0;
    WINDOWPLACEMENT windowedPlacement_{};

	// フルスクリーン制御用関数
	// フルスクリーンモードにする
    void EnterBorderlessFullscreen();
	// フルスクリーンモード解除
    void ExitBorderlessFullscreen();
	// クライアントサイズ更新
    void UpdateClientSize();

	// ウィンドウクラス登録
    void RegisterWindowClass();
	// マウスの生入力登録
    void RegisterMouseRawInput(HWND hwnd);
	// メインウィンドウ生成
    void CreateMainWindow(int width, int height, const std::wstring& title);
};
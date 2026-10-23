#pragma once
#include <Windows.h>
#include <string>

class MouseController;

/// <summary>
/// ウィンドウ管理クラス
/// </summary>
class WindowManager {
public:
    WindowManager(uint32_t width, uint32_t height, const std::wstring& title);
    ~WindowManager();
    void SetMouseController(MouseController* mc);
	void Update();


	// フルスクリーン制御
    void SetFullscreen(bool enable);
    void ToggleFullscreen();
    bool IsFullscreen() const { return isFullscreen_; }

    // 終了
    void Quit();

    // 前面にあって、かつ最小化されていない
    bool IsActive() const { return ::GetForegroundWindow() == hwnd_ && !::IsIconic(hwnd_); }

	// ウィンドウサイズ取得
	uint32_t GetWindowWidth() const { return winWidth_; }
	uint32_t GetWindowHeight() const { return winHeight_; }

	// ウィンドウハンドル取得
    HWND GetHwnd() const { return hwnd_; }

private:
	MouseController* mouseController_ = nullptr;

    // Windowsから呼ばれる入口
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    // ウィンドウプロシージャの本体
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    HWND hwnd_ = nullptr;
    WNDCLASS wc_{};

    // ウィンドウサイズ
    uint32_t winWidth_;
    uint32_t winHeight_;

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

	// ウィンドウクラス登録
    void RegisterWindowClass();
	// マウスの生入力登録
    void RegisterMouseRawInput(HWND hwnd);
	// メインウィンドウ生成
    void CreateMainWindow(LONG width, LONG height, const std::wstring& title);



    //LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

#include "Window/WindowManager.h"
#include "ImGuiManager/ImGuiManager.h"
#include "IO/MouseController.h"
#include <vector>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#pragma comment(lib, "winmm.lib")

uint32_t WindowManager::winWidth_;
uint32_t WindowManager::winHeight_;

// ウィンドウプロシージャ(クリックした、×を押した等のイベントを処理する関数)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    // ImGuiの入力を優先的に処理（UI操作中はゲーム側へイベントを流さない）
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return TRUE; }

    switch (msg)
    {
    // マウスが動いた時
    case WM_INPUT:
    {
        // Raw Inputから相対マウス移動を取得
        UINT size = 0;
        if (::GetRawInputData((HRAWINPUT)lparam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == 0 && size)
        {
            std::vector<BYTE> buf(size);
            if (::GetRawInputData((HRAWINPUT)lparam, RID_INPUT, buf.data(), &size, sizeof(RAWINPUTHEADER)) == size)
            {
                RAWINPUT* ri = reinterpret_cast<RAWINPUT*>(buf.data());
                if (ri->header.dwType == RIM_TYPEMOUSE)
                {
                    auto* mc = reinterpret_cast<MouseController*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
                    if (mc)
                    {
						// 相対移動量をマウスコントローラに送る
                        mc->OnRawMouseDelta(ri->data.mouse.lLastX, ri->data.mouse.lLastY);

						// ホイール回転量をマウスコントローラに送る
                        if (ri->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
                        {
                            int32_t wheelDelta = static_cast<SHORT>(ri->data.mouse.usButtonData);
                            mc->OnMouseWheelDelta(wheelDelta);
						}

                    }
                }
            }
        }
        return 0;
    }

    // ウィンドウがフォーカス
    case WM_ACTIVATE:
    {
        // アクティブ化
        if (wparam != WA_INACTIVE)
        {
            // アクティブフラグを立てる
            auto* wm = reinterpret_cast<WindowManager*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
            if (wm)
            {
                wm->isActive_ = true;
            }
        }
        // 非アクティブ化
        else
        {
            // アクティブフラグを下ろす
            auto* wm = reinterpret_cast<WindowManager*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
            if (wm)
            {
                wm->isActive_ = false;
            }
        }
		return 0;
	}

    // ウィンドウが破壊された時
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    // 既定のメッセージ処理
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

WindowManager::WindowManager(int width, int height, const std::wstring& title)
{
	winWidth_ = width;
	winHeight_ = height;
	timeBeginPeriod(1); // タイマー精度を1msに設定
    RegisterWindowClass();
    CreateMainWindow(width, height, title);
}

WindowManager::~WindowManager()
{
    // フルスクリーン中なら復帰してから破棄
    if (isFullscreen_)
    {
        ExitBorderlessFullscreen();
    }
    // DestroyWindowでウィンドウ破壊
    if (hwnd_)
    {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
}

void WindowManager::AttachMouseController(MouseController* mc)
{
    ::SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(mc));
}

void WindowManager::RegisterWindowClass() {
    // ウィンドウクラス作成
    wc_ = {};
    // ウィンドウプロシージャ
    wc_.lpfnWndProc = WindowProc;
    // ウィンドウクラス名
    wc_.lpszClassName = L"CG2WindowClass";
    // インスタンスハンドル
    wc_.hInstance = GetModuleHandle(nullptr);
    // カーソル
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);
    // ウィンドウクラスを登録する
    RegisterClass(&wc_);
}

void WindowManager::RegisterMouseRawInput(HWND hwnd)
{
    RAWINPUTDEVICE rid{};
    rid.usUsagePage = 0x01;     // Generic Desktop
    rid.usUsage = 0x02;         // Mouse
    rid.dwFlags = 0;            // 非アクティブでも受けるなら RIDEV_INPUTSINK
    rid.hwndTarget = hwnd;
    ::RegisterRawInputDevices(&rid, 1, sizeof(rid));
}

void WindowManager::CreateMainWindow(int width, int height, const std::wstring& title) 
{
    // ウィンドウサイズを表す構造体にクライアント領域を入れる
    RECT wrc = { 0,0,width,height };
    // クライアント領域を元に実際のサイズのwrcを変更してもらう
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    // ウィンドウの生成
    hwnd_ = CreateWindow(
        wc_.lpszClassName,		// 利用するウィンドウクラス名
        title.c_str(), 			// タイトルバーに表示する文字
        WS_OVERLAPPEDWINDOW,	// ウィンドウスタイルの選択
        CW_USEDEFAULT,			// 表示X座標
        CW_USEDEFAULT,			// 表示Y座標
        wrc.right - wrc.left,	// ウィンドウ横幅
        wrc.bottom - wrc.top,	// ウィンドウ縦幅
        nullptr,				// 親ウィンドウハンドル
        nullptr,				// メニューハンドル
        wc_.hInstance,			// インスタンスハンドル
        nullptr					// オプション
    );

	// ウィンドウ表示
    ShowWindow(hwnd_, SW_SHOW);

    // 
    RegisterMouseRawInput(hwnd_);
}

void WindowManager::SetFullscreen(bool enable)
{
	// すでにその状態なら何もしない
    if (enable == isFullscreen_) return;
	// フルスクリーン切り替え
    if (enable)
    {
        EnterBorderlessFullscreen();
    }
	// 解除
    else
    {
        ExitBorderlessFullscreen();
    }
	// ウィンドウサイズ更新
	UpdateClientSize();
}

void WindowManager::ToggleFullscreen()
{
    SetFullscreen(!isFullscreen_);
}

void WindowManager::Quit()
{
	PostQuitMessage(0);
}

// isFullscreen = trueになる
void WindowManager::EnterBorderlessFullscreen()
{
    if (!hwnd_) return;

    // 現在のウィンドウ情報を保存
    windowedStyle_ = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_STYLE));
    windowedExStyle_ = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_EXSTYLE));
    windowedPlacement_.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hwnd_, &windowedPlacement_);

    // 対象モニタのワークエリアではなくモニタ全体を使用
    HMONITOR hMon = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMon, &mi);

    // ボーダーレスにしてモニタ全体へフィット
    SetWindowLongPtr(hwnd_, GWL_STYLE, windowedStyle_ & ~(WS_OVERLAPPEDWINDOW));
    SetWindowLongPtr(hwnd_, GWL_EXSTYLE, windowedExStyle_ | WS_EX_APPWINDOW);

    SetWindowPos(
        hwnd_,
        HWND_TOP,
        mi.rcMonitor.left,
        mi.rcMonitor.top,
        mi.rcMonitor.right - mi.rcMonitor.left,
        mi.rcMonitor.bottom - mi.rcMonitor.top,
        SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW
    );

    isFullscreen_ = true;
}

// isFullscreen = falseになる
void WindowManager::ExitBorderlessFullscreen()
{
    if (!hwnd_) return;

    // 元のスタイルへ戻す
    SetWindowLongPtr(hwnd_, GWL_STYLE, windowedStyle_);
    SetWindowLongPtr(hwnd_, GWL_EXSTYLE, windowedExStyle_);

    // ウィンドウ配置と枠を復元
    SetWindowPlacement(hwnd_, &windowedPlacement_);
    SetWindowPos(
        hwnd_,
        nullptr,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW
    );

    isFullscreen_ = false;
}

void WindowManager::UpdateClientSize()
{
    if (!hwnd_) return;
    RECT rc{};
    GetClientRect(hwnd_, &rc);
    winWidth_ = static_cast<uint32_t>(rc.right - rc.left);
    winHeight_ = static_cast<uint32_t>(rc.bottom - rc.top);
}
#include "IO/MouseController.h"
#include "Utilities/functions.h"
#include "Window/WindowManager.h"
#include "Camera/CameraManager.h"
#include <ImGuiManager/ImGuiManager.h>

MouseController::MouseController(HWND hwnd, CameraManager* cameraManager)
	:cameraManager_(cameraManager)
{
    hwnd_ = hwnd;
    wheelDelta_ = 0;
    isVisible_ = true;
}

void MouseController::Update()
{
    // マウスポジション取得
    UpdatePosition();

    // マウスレイ取得
    UpdateRay();

    // マウスボタン状態取得
    UpdateButtonState();

	// マウス感度適用
	UpdateSensitivity();
}

void MouseController::EndFrame()
{
	// マウスホイール回転量リセット
    wheelDelta_ = 0;

    // マウス相対移動量リセット
    rawDelta_ = Vector2{ 0,0 }; 
}

// 今押しているか  i: 0=左ボタン、1=右ボタン、2=中ボタン
bool MouseController::IsHeld(int i)
{
    switch (i)
    {
    case 0:
        return leftButton_.curr;
    case 1:
        return rightButton_.curr;
    case 2:
        return middleButton_.curr;
    default:
        return false;
    }
}

// 押した瞬間（今フレームで押された） i: 0=左ボタン、1=右ボタン、2=中ボタン
bool MouseController::IsJustPressed(int i)
{
    switch (i)
    {
    case 0:
        return (!leftButton_.prev && leftButton_.curr);
    case 1:
        return (!rightButton_.prev && rightButton_.curr);
    case 2:
        return (!middleButton_.prev && middleButton_.curr);
    default:
        return false;
    }
}

// 離した瞬間（今フレームで離れた） i: 0=左ボタン、1=右ボタン、2=中ボタン
bool MouseController::IsJustReleased(int i)
{
    switch (i)
    {
    case 0:
        return (leftButton_.prev && !leftButton_.curr);
    case 1:
        return (rightButton_.prev && !rightButton_.curr);
    case 2:
        return (middleButton_.prev && !middleButton_.curr);
    default:
        return false;
    }
}

// 押されてからの経過フレーム数 i: 0=左ボタン、1=右ボタン、2=中ボタン
uint32_t MouseController::HoldFrames(int i)
{
    switch (i)
    {
    case 0:
        return leftButton_.holdFrames;
    case 1:
        return rightButton_.holdFrames;
    case 2:
        return middleButton_.holdFrames;
    default:
        return 0;
    }
}

// マウスカーソルの表示・非表示切り替え
void MouseController::ToggleMouseCursorVisible()
{
    if (!isVisible_)
    {
        // カーソルを表示
        ShowCursor(TRUE);
    }
    else
    {
        // カーソルを非表示
        ShowCursor(FALSE);
	}
}

// マウスカーソルの表示・非表示設定
void MouseController::ShowCursor(bool visible)
{
	isVisible_ = visible;
    if (visible)
    {
        // カウンタが負になるまで表示側へ（1024敗）
        while (::ShowCursor(TRUE) < 0) {}
    }
    else
    {
        // カウンタが負になるまで非表示側へ
        while (::ShowCursor(FALSE) >= 0) {}
    }
}

// マウスポジション取得
void MouseController::UpdatePosition()
{

    // ゲームウィンドウが非アクティブならロックしない
    const bool isAppFocused = (::GetForegroundWindow() == hwnd_);

	ImGui::Begin("Mouse Position");

	// 非表示 && ウィンドウアクティブ時は画面中央にロック
    if (!isVisible_ && isAppFocused)
    {
        RECT rc{};
        ::GetClientRect(hwnd_, &rc);

        POINT center{};
        center.x = (rc.left + rc.right) / 2;
        center.y = (rc.top + rc.bottom) / 2;

        // クライアント座標 -> スクリーン座標へ変換してから SetCursorPos
        ::ClientToScreen(hwnd_, &center);
        ::SetCursorPos(center.x, center.y);

        // position_ はクライアント座標で保持している前提なので、ここはクライアント中心値にする
        position_ = Vector2{ float((rc.left + rc.right) / 2), float((rc.top + rc.bottom) / 2) };

        ImGui::Text("Cursor Locked to Center");
        ImGui::End();
        return;
	}

    // hwnd: ゲームウィンドウのハンドル（WindowManagerなどから取得）
    POINT mousePosScreen;
    ::GetCursorPos(&mousePosScreen); // 画面座標で取得

    // クライアント座標（ウィンドウ左上基準）に変換
    ::ScreenToClient(hwnd_, &mousePosScreen);

    // mousePosScreen.x, mousePosScreen.y がウィンドウ内のマウス座標
    position_ = Vector2{ float(mousePosScreen.x),float(mousePosScreen.y) };

    ImGui::Text("Cursor UnLocked");
    ImGui::End();
}

// マウスレイ取得
void MouseController::UpdateRay()
{
    // 左下が０、右上が１とした時のマウスポジション
    float ndcX = (position_.x / WindowManager::winWidth_) * 2.0f - 1.0f;
    float ndcY = 1.0f - (position_.y / WindowManager::winHeight_) * 2.0f; // Yは上下反転

    // クリップ空間でZ=0(near)とZ=1(far)の2点を作る
    Vector4 nearPoint = { ndcX, ndcY, 0.0f, 1.0f };
    Vector4 farPoint = { ndcX, ndcY, 1.0f, 1.0f };

    // 逆射影行列
    Matrix4x4 inverseViewProj = cameraManager_->GetCurrentViewProjectionMatrix().Inverse();

    // ワールド空間に変換
    Vector4 nearWorld = Transform(nearPoint, inverseViewProj);
    Vector4 farWorld = Transform(farPoint, inverseViewProj);

    // マウスレイの始点・方向
    ray_.origin = { nearWorld.x / nearWorld.w, nearWorld.y / nearWorld.w, nearWorld.z / nearWorld.w };
    ray_.diff = Vector3{
    (farWorld.x / farWorld.w) - ray_.origin.x,
    (farWorld.y / farWorld.w) - ray_.origin.y,
    (farWorld.z / farWorld.w) - ray_.origin.z
    }.Normalized();
}

// マウスボタン状態取得
void MouseController::UpdateButtonState()
{
    leftButton_.prev = leftButton_.curr;
	rightButton_.prev = rightButton_.curr;
	middleButton_.prev = middleButton_.curr;

    leftButton_.curr = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    rightButton_.curr = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    middleButton_.curr = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;

    if (leftButton_.curr)leftButton_.holdFrames++;
    else leftButton_.holdFrames = 0;
    if (rightButton_.curr) rightButton_.holdFrames++;
    else rightButton_.holdFrames = 0;
    if (middleButton_.curr) middleButton_.holdFrames++;
    else middleButton_.holdFrames = 0;
}

// マウス感度の適用
void MouseController::UpdateSensitivity()
{
	rawDelta_ *= mouseSensitivity_;
}


// 相対移動の蓄積
void MouseController::OnRawMouseDelta(int dx, int dy)
{
    rawDelta_.x += static_cast<float>(dx);
    rawDelta_.y += static_cast<float>(dy);
}
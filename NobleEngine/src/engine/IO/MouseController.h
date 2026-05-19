#pragma once
#include "definition/definition.h"

class CameraManager;

/// <summary>
/// マウス管理クラス
/// </summary>
class MouseController
{
public:
	MouseController(HWND hwnd, CameraManager* cameraManager);
	void Update();
	void EndFrame();

	void UpdatePosition();		// マウスポジション更新
	void UpdateRay();			// マウスレイ更新
	void UpdateButtonState();	// マウスボタン状態更新
	void UpdateSensitivity();	// マウス感度の適用

	void SetSensitivity(float sensitivity) { mouseSensitivity_ = sensitivity; }	// マウス感度設定

	// 相対移動の蓄積
	void OnRawMouseDelta(int dx, int dy);	
	// ホイール回転量の蓄積
	void OnMouseWheelDelta(int delta) { wheelDelta_ += delta; }

	int32_t GetWheelDelta() const { return wheelDelta_; }		// マウスホイール回転量取得
	Vector2 GetRawDelta() const { return rawDelta_; }			// マウス相対移動を取得
	Vector2 GetPosition() const { return position_; }			// マウス2D座標取得
	Vector3 GetWorldPosition() const { return ray_.origin; }	// マウス3D座標取得
	Ray GetRay() const { return ray_; }							// マウスレイ取得

	bool IsHeld(int i);			// 今押しているか
	bool IsJustPressed(int i);	// 押した瞬間（今フレームで押された）
	bool IsJustReleased(int i);	// 離した瞬間（今フレームで離れた）
	uint32_t HoldFrames(int i);	// 押されてからの経過フレーム数

	void ToggleMouseCursorVisible();
	void ShowCursor(bool visible);


private:

	// カーソル表示フラグ
	bool isVisible_;

	// マウスボタン状態
	mouseButtonState leftButton_;
	mouseButtonState rightButton_;
	mouseButtonState middleButton_;

	// マウス移動量感度
	float mouseSensitivity_ = 1.0f;

	
	int32_t wheelDelta_ = 0;	// マウスホイール回転量 （1フレーム分の合計）
	Vector2 rawDelta_{ 0,0 };	// マウス相対移動量		（1フレーム分の合計）
	Vector2 position_;			// マウス2D座標
	Ray ray_;					// マウスレイ

	HWND hwnd_;					// ウィンドウハンドル

	// マウスレイ計算に必要なViewProjectionMatrix取得用
	CameraManager* cameraManager_;
};
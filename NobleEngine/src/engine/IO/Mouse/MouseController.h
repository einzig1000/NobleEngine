#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <optional>

class CameraManager;

/// <summary>
/// マウス管理クラス
/// </summary>
class MouseController
{
public:
	MouseController(HWND hwnd);
	void Update();
	void EndFrame();

	void SetSensitivity(float sensitivity) { mouseSensitivity_ = sensitivity; }	// マウス感度設定

	// 相対移動の蓄積
	void OnRawMouseDelta(int32_t dx, int32_t dy);	
	// ホイール回転量の蓄積
	void OnMouseWheelDelta(int32_t delta) { wheelDelta_ += delta; }

	int32_t GetWheelDelta() const { return wheelDelta_; }		// マウスホイール回転量取得
	Vector2 Get2DPositionDelta() const { return rawDelta_; }	// 前フレームとのマウス相対移動を取得
	Vector2 Get2DPosition() const { return position_; }			// マウス2D座標取得
	Vector3 GetWorldPosition(Matrix4x4& viewProjection);				// マウス3D座標取得
	Ray GetRay(Matrix4x4& viewProjection);							// マウスレイ取得

	bool IsHeld(int32_t i);			// 今押しているか
	bool IsJustPressed(int32_t i);	// 押した瞬間（今フレームで押された）
	bool IsJustReleased(int32_t i);	// 離した瞬間（今フレームで離れた）
	uint32_t HoldFrames(int32_t i);	// 押されてからの経過フレーム数

	void ToggleMouseCursorVisible();
	void ShowCursor(bool visible);

private:
	void UpdateButtonState();	// マウスボタン状態更新
	void UpdateSensitivity();	// マウス感度の適用

	void Compute2DPosition();	// マウス2Dポジション計算
	Ray ComputeRay(Matrix4x4& viewProjection);	// マウスレイ計算

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

	HWND hwnd_;					// ウィンドウハンドル

	// マウスレイ計算に必要なViewProjectionMatrix取得用
	CameraManager* cameraManager_;
};
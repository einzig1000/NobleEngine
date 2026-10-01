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

	// マウス感度設定
	void SetSensitivity(float sensitivity) { mouseSensitivity_ = sensitivity; }

	// 相対移動距離の蓄積
	void OnRawMouseDelta(int32_t dx, int32_t dy);	
	// ホイール回転量の蓄積
	void OnMouseWheelDelta(int32_t delta) { wheelDelta_ += delta; }


	bool IsHeld(int32_t i);			// 今押しているか
	bool IsJustPressed(int32_t i);	// 押した瞬間（今フレームで押された）
	bool IsJustReleased(int32_t i);	// 離した瞬間（今フレームで離れた）
	float HoldSeconds(int32_t i);	// 押されてからの経過秒数

	// マウスカーソルの表示・非表示切り替え
	void ToggleMouseCursorVisible();
	// マウスカーソルの表示・非表示設定
	void ShowCursor(bool visible);


	/// <summary>
	/// マウスホイール回転量を取得する
	/// </summary>
	/// <returns>マウスホイール回転量</returns>
	int32_t GetWheelDelta() const { return wheelDelta_; }		

	/// <summary>
	/// 前フレームとのマウス相対移動量を取得
	/// </summary>
	/// <returns>前フレームとのマウス相対移動量</returns>
	Vector2 Get2DPositionDelta() const { return rawDelta_; }

	/// <summary>
	/// マウスの2D座標を取得する
	/// </summary>
	/// <returns>マウスの2D座標</returns>
	Vector2 Get2DPosition() const { return position_; }

	/// <summary>
	/// マウスのワールド座標を取得する
	/// </summary>
	/// <param name="viewProjection">カメラのビュー射影行列</param>
	/// <returns>マウスのワールド座標</returns>
	Vector3 Get3DPosition(Matrix4x4& viewProjection);

	/// <summary>
	/// マウスレイを取得する
	/// </summary>
	/// <param name="viewProjection">カメラのビュー射影行列</param>
	/// <returns>マウスレイ</returns>
	Ray GetRay(Matrix4x4& viewProjection);

private:
	void UpdateButtonState();	// マウスボタン状態更新
	void UpdateSensitivity();	// マウス感度の適用

	void UpdateCursorLock();				// 表示要求+フォーカス状態からロック要否を毎フレーム再評価
	void ApplyCursorLock(bool locked);		// ロック要否を実際にOSへ反映(true=非表示+クリップ／false=表示+クリップ解除)

	void Compute2DPosition();	// マウス2Dポジション計算
	Ray ComputeRay(Matrix4x4& viewProjection);	// マウスレイ計算

	// カーソル表示フラグ
	bool isVisible_;
	bool isCursorLocked_;	// 直近に適用した状態(true=非表示+クリップ中)

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
};
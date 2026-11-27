#pragma once
#include <Camera/CameraPose.h>

/// <summary>
/// 1フレーム分のカメラ操作(どのボタンが何の操作かは Camera が決める)
/// </summary>
struct CameraInput
{
	// マウスの移動量
	Vector2 mouseDelta = { 0.0f, 0.0f };
	// ホイールの回転量
	int32_t wheel = 0;
	// 向きを変える操作をしているか
	bool isRotating = false;
	// 平行移動の操作をしているか
	bool isPanning = false;
};

/// <summary>
/// カメラの操作モード(State Pattern の状態)の共通インターフェース
/// Camera からは姿勢(CameraPose)だけを受け取り、行列やイージングには触らない
/// </summary>
class ICameraState
{
public:
	virtual ~ICameraState() = default;

	// 操作を姿勢に反映する(マウスによる操作が可能なときだけイージングより前に呼ばれる)
	virtual void HandleInput(CameraPose& pose, const CameraInput& input) = 0;
	// 向きと距離からeyeとcenterを決める(イージングと範囲の制限の後に呼ばれる)
	virtual void ResolvePose(CameraPose& pose) const = 0;
	// どの操作モードか
	virtual CameraMode_ORBIT_FPS GetMode() const = 0;
};


#include "OrbitCameraState.h"

namespace
{
	// マウス1ピクセルあたりの回転量(ラジアン)
	constexpr float kRotateSensitivity = 0.01f;
	// マウス1ピクセルあたりの平行移動量(距離に掛ける。距離10なら0.02)
	constexpr float kPanSensitivity = 0.002f;
	// ホイール1あたりに距離を縮める割合
	constexpr float kZoomSensitivity = 0.001f;
}

void OrbitCameraState::HandleInput(CameraPose& pose, const CameraInput& input)
{
	// 平行移動:centerを画面の右・上方向に動かす
	if (input.isPanning)
	{
		const float speed = pose.spherical.radius * kPanSensitivity;
		pose.center += (pose.GetRight() * input.mouseDelta.x + pose.GetUp() * input.mouseDelta.y) * speed;
	}

	// 回転: centerの周りをeyeが回る
	if (input.isRotating)
	{
		pose.spherical.phi += input.mouseDelta.y * kRotateSensitivity;
		pose.spherical.theta -= input.mouseDelta.x * kRotateSensitivity;
	}

	// ホイール: centerに近づく・離れる
	pose.spherical.radius -= input.wheel * pose.spherical.radius * kZoomSensitivity;
}

void OrbitCameraState::ResolvePose(CameraPose& pose) const
{
	pose.eye = pose.center + pose.GetOffset();
}

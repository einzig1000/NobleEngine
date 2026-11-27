#include "FpsCameraState.h"

namespace
{
	// マウス1ピクセルあたりの回転量(ラジアン)
	constexpr float kRotateSensitivity = 0.01f;
	// マウス1ピクセルあたりの平行移動量(距離に掛ける。距離10なら0.02)
	constexpr float kPanSensitivity = 0.002f;
	// ホイール1あたりに前へ進む量(距離に掛ける。ORBIT のズームと同じ量だけeyeが動く)
	constexpr float kMoveSensitivity = 0.001f;
}

void FpsCameraState::HandleInput(CameraPose& pose, const CameraInput& input)
{
	// 平行移動:eyeを画面の右・上方向に動かす
	if (input.isPanning)
	{
		const float speed = pose.spherical.radius * kPanSensitivity;
		pose.eye += (pose.GetRight() * input.mouseDelta.x + pose.GetUp() * input.mouseDelta.y) * speed;
	}

	// 回転:eyeの位置はそのままで見る向きだけを変える
	if (input.isRotating)
	{
		pose.spherical.phi += input.mouseDelta.y * kRotateSensitivity;
		pose.spherical.theta -= input.mouseDelta.x * kRotateSensitivity;
	}

	// ホイール:見ている方向へ進む・下がる
	pose.eye += pose.GetForward() * (input.wheel * pose.spherical.radius * kMoveSensitivity);
}

void FpsCameraState::ResolvePose(CameraPose& pose) const
{
	pose.center = pose.eye - pose.GetOffset();
}

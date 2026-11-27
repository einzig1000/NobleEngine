#pragma once
#include <Camera/CameraState/ICameraState.h>

/// <summary>
/// FPS: カメラ(eye)を中心にcenterが回る
/// </summary>
class FpsCameraState : public ICameraState
{
public:
	void HandleInput(CameraPose& pose, const CameraInput& input) override;
	void ResolvePose(CameraPose& pose) const override;
	CameraMode_ORBIT_FPS GetMode() const override { return CameraMode_ORBIT_FPS::FPS; }
};

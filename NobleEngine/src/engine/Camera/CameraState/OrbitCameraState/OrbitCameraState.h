#pragma once
#include <Camera/CameraState/ICameraState.h>

/// <summary>
/// ORBIT: centerを中心にカメラ(eye)が回る
/// </summary>
class OrbitCameraState : public ICameraState
{
public:
	void HandleInput(CameraPose& pose, const CameraInput& input) override;
	void ResolvePose(CameraPose& pose) const override;
	CameraMode_ORBIT_FPS GetMode() const override { return CameraMode_ORBIT_FPS::ORBIT; }
};

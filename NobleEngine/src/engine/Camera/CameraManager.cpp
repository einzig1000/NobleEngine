#include "CameraManager.h"
#include "Camera.h"
#include <algorithm>
#include <ImGuiManager/ImGuiManager.h>

CameraManager::CameraManager()
{
	Camera ReleaseCamera;
	ReleaseCamera.name_ = "ReleaseCamera";
	ReleaseCamera.SetEnableControl(false);
	camera_.push_back(ReleaseCamera);

	Camera DebugCamera;
	DebugCamera.name_ = "DebugCamera";
	DebugCamera.SetEnableControl(true);
	camera_.push_back(DebugCamera);
}

CameraManager::~CameraManager()
{
}

void CameraManager::AddCamera(const std::string name, bool enableControl)
{
	// 名前が被ってないか確認
	if (std::any_of(camera_.begin(), camera_.end(), [&](const Camera& cam) { return cam.name_ == name; }))
	{
		assert(false && "同じ名前のカメラが既に存在しています");
		return;
	}

	Camera def;
	def.name_ = name;
	def.SetEnableControl(enableControl);
	camera_.push_back(def);
}

void CameraManager::Update()
{
	camera_[currentCameraID_].Update();
}

void CameraManager::Draw()
{
	ImGui::Begin("Camera");
	camera_[currentCameraID_].DrawImGui();
	ImGui::End();

	camera_[currentCameraID_].Draw();
}

void CameraManager::Resize()
{
	for (auto& cam : camera_)
	{
		cam.Resize();
	}
}


void CameraManager::SetCenterTarget(Vector3 Center, int spendFrame, EaseType easetype)
{
	camera_[currentCameraID_].SetCenterTarget(Center, spendFrame, easetype);
}
void CameraManager::SetRotateTarget(Vector3 Center, int spendFrame, EaseType easetype)
{
	camera_[currentCameraID_].SetRotateTarget(Center, spendFrame, easetype);
}
void CameraManager::SetDistanceTarget(float Center, int spendFrame, EaseType easetype)
{
	camera_[currentCameraID_].SetDistanceTarget(Center, spendFrame, easetype);
}


void CameraManager::StartShake(float intensity, float duration, float frequency)
{
	camera_[currentCameraID_].StartShake(intensity, duration, frequency);
}
bool CameraManager::IsShaking()
{
	return camera_[currentCameraID_].IsShaking();
}
void CameraManager::StopShake()
{
	camera_[currentCameraID_].StopShake();
}


void CameraManager::SetEnableControl(bool enable)
{
	camera_[currentCameraID_].SetEnableControl(enable);
}


Vector3 CameraManager::GetCenter(const std::string name) const
{
	for (const auto& cam : camera_)
	{
		if (cam.name_ == name)
		{
			return cam.GetCenter();
		}
	}

	return Vector3{};
}
Vector3 CameraManager::GetTranslate(const std::string name) const
{
	for (const auto& cam : camera_)
	{
		if (cam.name_ == name)
		{
			return cam.GetTranslate();
		}
	}

	return Vector3{};
}
Matrix4x4 CameraManager::GetViewProjectionMatrix(const std::string name) const
{
	for (const auto& cam : camera_)
	{
		if (cam.name_ == name)
		{
			return cam.GetViewProjectionMatrix();
		}
	}

	return Matrix4x4{};
}
float CameraManager::GetDistance(const std::string name) const
{
	for (const auto& cam : camera_)
	{
		if (cam.name_ == name)
		{
			return cam.GetDistance();
		}
	}

	return float{};
}


Vector3 CameraManager::GetCurrentCenter() const
{
	return camera_[currentCameraID_].GetCenter();
}
Vector3 CameraManager::GetCurrentTranslate() const
{
	return camera_[currentCameraID_].GetTranslate();
}
Vector3 CameraManager::GetCurrentRotate() const
{
	return camera_[currentCameraID_].GetRotate();
}
Matrix4x4 CameraManager::GetCurrentViewProjectionMatrix() const
{
	return camera_[currentCameraID_].GetViewProjectionMatrix();
}
Matrix4x4 CameraManager::GetCurrentViewMatrix() const
{
	return camera_[currentCameraID_].GetViewMatrix();
}
Matrix4x4 CameraManager::GetCurrentProjectionMatrix() const
{
	return camera_[currentCameraID_].GetProjectionMatrix();
}
float CameraManager::GetCurrentDistance() const
{
	return camera_[currentCameraID_].GetDistance();
}


void CameraManager::SetCameraMode(CameraMode_ORBIT_FPS mode)
{
	camera_[currentCameraID_].SetCameraMode(mode);
}

bool CameraManager::InCamera(const AABB& aabb)
{
	return camera_[currentCameraID_].InFrustum(aabb);
}

void CameraManager::ToggleCamera()
{
	currentCameraID_++;
	if (currentCameraID_ >= static_cast<int>(camera_.size()))
	{
		currentCameraID_ = 0;
	}
}

void CameraManager::ToggleCamera(const std::string name)
{
	for (size_t i = 0; i < camera_.size(); ++i)
	{
		if (camera_[i].name_ == name)
		{
			currentCameraID_ = static_cast<int>(i);
			return;
		}
	}
	assert(false && "指定された名前のカメラが存在しません");
}

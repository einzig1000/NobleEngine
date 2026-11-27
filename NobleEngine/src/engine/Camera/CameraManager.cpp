#include "CameraManager.h"
#include <Camera/Camera.h>
#include <ImGuiManager/ImGuiManager.h>

CameraManager::CameraManager(IOManager* ioManager)
	: ioManager_(ioManager)
{
}

CameraManager::~CameraManager()
{
}

int32_t CameraManager::AddCamera(const std::string& name)
{
	std::string cameraName = name;
	if (cameraName == "")
	{
		cameraName = "Camera" + std::to_string(camera_.size());
	}
	Camera& camera = camera_.emplace_back(ioManager_);
	camera.SetName(cameraName);
	return static_cast<int32_t>(camera_.size() - 1);
}

void CameraManager::Update(int32_t cameraID)
{
	Camera* targetCamera = GetCamera(cameraID);
	if (!targetCamera) return;

	targetCamera->Update();
}

void CameraManager::DrawImGui()
{
	ImGui::Begin("CameraManager");
	if (ImGui::BeginTabBar("Camera", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable))
	{
		for (size_t i = 0; i < camera_.size(); i++)
		{
			std::string name = camera_[i].GetName();
			if (ImGui::BeginTabItem(name.c_str()))
			{
				camera_[i].DrawImGui();
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

Camera* CameraManager::GetCamera(int32_t cameraID)
{
	if (cameraID < 0 || cameraID >= static_cast<int32_t>(camera_.size()))
	{
		return nullptr;
	}
	return &camera_[cameraID];
}

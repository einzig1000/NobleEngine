#include "CameraManager.h"
#include <Camera/Camera.h>
#include <ImGuiManager/ImGuiManager.h>

CameraManager::CameraManager()
{
}

CameraManager::~CameraManager()
{
}

int32_t CameraManager::AddCamera()
{
	Camera def;
	camera_.push_back(def);
	return static_cast<int32_t>(camera_.size() - 1);
}

void CameraManager::Update(const int32_t cameraID)
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
			std::string name = "Camera " + std::to_string(i);
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

Camera* CameraManager::GetCamera(const int32_t cameraID)
{
	if (cameraID < 0 || cameraID >= static_cast<int32_t>(camera_.size()))
	{
		return nullptr;
	}
	return &camera_[cameraID];
}

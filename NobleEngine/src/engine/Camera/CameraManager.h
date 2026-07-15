#pragma once
#include <EngineDefinition/EngineDefinition.h>

class Camera;

class CameraManager
{
public:
	CameraManager();
	~CameraManager();

	int32_t AddCamera(const std::string& name);

	void Update(const int32_t cameraID);
	void DrawImGui();

	Camera* GetCamera(const int32_t cameraID);

private:
	std::vector<Camera> camera_;
};
 
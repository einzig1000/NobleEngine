#pragma once
#include <EngineDefinition/EngineDefinition.h>

class Camera;
class IOManager;

class CameraManager
{
public:
	CameraManager(IOManager* ioManager);
	~CameraManager();

	int32_t AddCamera(const std::string& name);

	void Update(int32_t cameraID);
	void DrawImGui();

	Camera* GetCamera(int32_t cameraID);

private:
	IOManager* ioManager_ = nullptr;

	std::vector<Camera> camera_;
};
 
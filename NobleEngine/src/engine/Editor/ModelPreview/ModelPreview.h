#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <memory>

class ModelBank;
class DirectXManager;
class CameraManager;

class ModelPreview
{
public:
	ModelPreview(DirectXManager* dxManager, CameraManager* cameraManager, ModelBank* bank);
	~ModelPreview();

	void Update();
	void Draw();
	void DrawImGui();

private:
	ModelBank* bank_;
	DirectXManager* dxManager_;
	CameraManager* cameraManager_;

	std::unique_ptr<RenderObject> modelRenderObject_;
	int32_t cameraID_ = -1;
	int32_t renderTarget_ = -1;
	int32_t textureID = -1;
	//QuaternionTransforms objectTransform_;
	EulerTransforms objectTransform_;

	bool fullscreen_ = false;
};


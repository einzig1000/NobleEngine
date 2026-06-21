#pragma once
#include <definition/definition.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <memory>

class ModelBank;
class DirectXManager;

class ModelEditor
{
public:
	ModelEditor(DirectXManager* dxManager, ModelBank* bank);
	~ModelEditor();

	void Draw();
	void DrawImGui();

private:
	ModelBank* bank_;
	DirectXManager* dxManager_;

	std::unique_ptr<RenderObject> modelRenderObject_;
	int32_t cameraID_ = -1;
	int32_t renderTarget_ = -1;
	Vector2 renderTargetSize_;
	EulerTransform objectTransform_;
};


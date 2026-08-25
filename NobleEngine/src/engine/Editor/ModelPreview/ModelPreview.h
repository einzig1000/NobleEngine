#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <memory>

class ModelBank;
class DirectXManager;
class CameraManager;

struct HitBoxInfo
{
	std::unique_ptr<RenderObject> renderObject;
	int32_t heapSlot = -1;
	std::vector<Vector3> vertices;
};

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

	int32_t cameraID_ = -1;
	int32_t renderTarget_ = -1;

	std::unique_ptr<RenderObject> modelRenderObject_;
	ModelData* modelData_ = nullptr;
	int32_t textureID = -1;
	EulerTransforms objectTransform_;

	bool fullscreen_ = false;

	void RebuildColliderRenderObjects();
	bool requestRebuildColliderRenderObjects_ = false;
	bool isEditingCollider_ = false;
	int32_t colliderCubeModelID_ = -1;
	int32_t colliderSphereModelID_ = -1;
	int32_t colliderTextureID_ = -1;
	std::vector<std::unique_ptr<RenderObject>> colliderRender_;
	ColliderShape colliderShape_;

};


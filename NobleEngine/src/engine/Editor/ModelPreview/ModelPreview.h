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
	const ModelData* modelData_ = nullptr;
	int32_t textureID = -1;
	EulerTransforms objectTransform_;

	bool fullscreen_ = false;

	void RebuildColliderRenderObjects();
	// コライダーの描画オブジェクト再構築(描画モデルが変更されたタイミング)
	bool requestRebuildColliderRenderObjects_ = false;
	// コライダー編集モードか
	bool isEditingCollider_ = false;
	// 選択中のコライダーのインデックス
	int32_t selectedColliderIndex_ = -1;
	// コライダーの描画用モデルID
	int32_t colliderCubeModelID_ = -1;
	int32_t colliderSphereModelID_ = -1;
	// コライダーの描画用テクスチャID
	int32_t colliderTextureID_ = -1;
	// 描画オブジェクト
	std::vector<std::unique_ptr<RenderObject>> colliderRender_;
	ColliderShape colliderShape_;

};


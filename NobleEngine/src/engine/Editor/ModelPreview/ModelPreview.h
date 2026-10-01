#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <DrawSystem/RenderObject/RenderObject.h>
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

	// モデル描画用オブジェクト
	std::unique_ptr<RenderObject> modelRenderObject_;
	const ModelData* modelData_ = nullptr;
	Matrix4x4 wpv_;
	Matrix4x4 world_;
	Vector4 color_;
	int32_t textureID = -1;
	EulerTransforms objectTransform_;

	// コライダー描画オブジェクト
	std::vector<std::unique_ptr<RenderObject>> colliderRender_;
	std::vector<Matrix4x4> colliderWpv_;
	std::vector<Matrix4x4> colliderWorld_;
	std::vector<Vector4> colliderColor_;
	ColliderShape colliderShape_;

	bool fullscreen_ = false;

	// コライダー編集モードのUI描画
	void RebuildColliderRenderObjects();
	// モデルを選択状態にし、関連データを更新する(リスト選択・ファイルダイアログ選択の共通処理)
	void SelectModel(int32_t modelID);
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
};


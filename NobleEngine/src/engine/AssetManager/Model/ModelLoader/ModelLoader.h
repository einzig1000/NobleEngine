#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>




class ModelBank;
class DirectXManager;

class ModelLoader
{
public:
	ModelLoader(DirectXManager* dxManager, ModelBank* bank);
	~ModelLoader();

	// モデル読み込み
	int32_t LoadModel(const std::string& filePath);

private:
	DirectXManager* dxManager_;
	ModelBank* bank_;

	// mtlファイル読み込み
	MaterialData LoadMaterialTemplateFile(const std::string& filePath);

	// モデルファイル読み込み
	void LoadModelFile(const std::string& filePath, ModelData* modelData);
	Node ReadNode(const aiNode* node);

	// SkinClusterを作成する
	SkinCluster CreateSkinCluster(const ModelData* modelData);

	// NodeからSkeletonを作成
	Skeleton CreateSkeleton(const Node& node);
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parentIndex, std::vector<Joint>& joints);

	// １，AABB読み込み
	std::vector<AABB> LoadAABB(const std::string& filePath, const std::vector<VertexData>& vertices);
	// ２、AABB.csvがあれば読み込み
	std::vector<AABB> LoadAABBFromCSV(const std::string& filePath);
	// ２，AABB.csvがなければモデルデータから作成
	AABB CreateLocalAABB(const std::vector<VertexData>& vertices);
	// ３、AABBをCSVに保存
	void SaveAABBToCSV(const std::string& filePath, const std::vector<AABB>& aabbs);


	// アップロード用一時リソースを保持するリスト
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateUploadResources_;
};


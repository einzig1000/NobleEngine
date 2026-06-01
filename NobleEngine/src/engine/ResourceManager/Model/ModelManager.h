#pragma once
#include <definition/definition.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/// <summary>
/// モデル管理クラス
/// </summary>
class ModelManager
{
public:
	ModelManager(ID3D12Device* device);
	~ModelManager();

	// モデル読み込み
	int32_t LoadModel(const std::string& filePath);

	int32_t CreateModel(const std::vector<VertexData>& vertices);

	Skeleton CreateSkeleton(const Node& node);

	// データ取得
	ModelData* GetModelData(int32_t modelID);

	// モデル数を取得
	size_t GetModelCount() const { return objects.size(); }

	// モデルリストを取得
	std::vector<ModelData>& GetModelList() { return objects; }

private:
	ID3D12Device* device_;

	// モデルデータを詰める
	std::vector<ModelData> objects;

	// mtlファイル読み込み
	MaterialData LoadMaterialTemplateFile(const std::string& filePath);

	// モデルファイル読み込み
	void LoadModelFile(const std::string& filePath, ModelData& modelData);

	// Node読み込み
	Node ReadNode(const aiNode* node);

	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parentIndex, std::vector<Joint>& joints);

	// １，AABB読み込み
	std::vector<AABB> LoadAABB(const std::string& filePath, const std::vector<VertexData>& vertices);

	// ２、AABB.csvがあれば読み込み
	std::vector<AABB> LoadAABBFromCSV(const std::string& filePath);
	
	// ２，AABB.csvがなければモデルデータから作成
	AABB CreateLocalAABB(const std::vector<VertexData>& vertices);

	// ３、AABBをCSVに保存
	void SaveAABBToCSV(const std::string& filePath, const std::vector<AABB>& aabbs);
};


#pragma once
#include <definition/definition.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



struct ResMeshlet
{
	uint32_t vertexOffset = 0;		// 頂点番号オフセット
	uint32_t vertexCount = 0;		// 頂点数
	uint32_t primitiveOffset = 0;	// プリミティブ番号オフセット
	uint32_t primitiveCount = 0;	// プリミティブ数
};

struct ResPrimitiveIndex
{
	uint32_t index = 0;	// 10bit * 3 = 30bit, 残り2bitは予約領域
};

struct ResMesh
{
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	uint32_t materialID = 0;

	std::vector<ResMeshlet> meshlets;
	std::vector<uint32_t> uniqueVertexIndices;
	std::vector<ResPrimitiveIndex> primitiveIndices;
};


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
	SkinCluster CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData);

	// NodeからSkeletonを作成
	Skeleton CreateSkeleton(const Node& node);
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parentIndex, std::vector<Joint>& joints);

	// モデルデータからメッシュレットを作成
	void CreateMeshlets(ResMesh& mesh, const aiMesh* aiMeshPtr);


	// １，AABB読み込み
	std::vector<AABB> LoadAABB(const std::string& filePath, const std::vector<VertexData>& vertices);
	// ２、AABB.csvがあれば読み込み
	std::vector<AABB> LoadAABBFromCSV(const std::string& filePath);
	// ２，AABB.csvがなければモデルデータから作成
	AABB CreateLocalAABB(const std::vector<VertexData>& vertices);
	// ３、AABBをCSVに保存
	void SaveAABBToCSV(const std::string& filePath, const std::vector<AABB>& aabbs);
};


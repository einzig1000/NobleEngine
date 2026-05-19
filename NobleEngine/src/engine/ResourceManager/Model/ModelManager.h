#pragma once
#include "definition/definition.h"

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

	// mtlファイルを読み込む関数
	MaterialData LoadMaterialTemplateFile(const std::string& filePath);

	// objファイルを読み込む関数
	std::vector<VertexData> LoadModelFile(const std::string& filePath);

	

	// １，AABB読み込み
	std::vector<AABB> LoadAABB(const std::string& filePath, const std::vector<VertexData>& vertices);

	// ２、AABB.csvがあれば読み込み
	std::vector<AABB> LoadAABBFromCSV(const std::string& filePath);
	
	// ２，AABB.csvがなければモデルデータから作成
	AABB CreateLocalAABB(const std::vector<VertexData>& vertices);

	// ３、AABBをCSVに保存
	void SaveAABBToCSV(const std::string& filePath, const std::vector<AABB>& aabbs);


};


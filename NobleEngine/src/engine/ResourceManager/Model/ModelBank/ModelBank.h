#pragma once
#include <definition/definition.h>
#include <unordered_map>
#include <memory>

class ModelBank
{
public:
	// モデルデータを追加
	void AddModelData(const std::string& filePath, const int32_t ID, std::unique_ptr<ModelData> modelData);
	// filePathが同じモデルデータが存在するか
	int32_t IsModelDataExist(std::string filePath) const;
	// 次のモデルIDを取得
	int32_t AllocateModelID() const { return static_cast<int32_t>(objects_.size()); }
	// modelIDからモデルデータを取得
	ModelData* GetModelData(int modelID);
	// モデルリストを取得
	std::vector<std::unique_ptr<ModelData>>& GetModelList() { return objects_; }

private:
	// キー：テクスチャのファイルパス、値：ID
	std::unordered_map<std::string, int32_t> pathToIDMap_;
	// キー：ID、値：テクスチャデータ
	std::vector<std::unique_ptr<ModelData>> objects_;
};


#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <unordered_map>
#include <memory>

class ModelBank
{
public:
	// モデルデータを追加
	int32_t AddModelData(const std::string& filePath, std::unique_ptr<ModelData> modelData);
	// filePathが同じモデルデータが存在するか
	int32_t IsModelDataExist(const std::string filePath) const;
	// modelIDからモデルデータを取得
	const ModelData* GetModelData(int32_t modelID) const;
	// モデルリストを取得
	const std::vector<std::unique_ptr<ModelData>>& GetModelList() const { return objects_; }

private:
	// キー：テクスチャのファイルパス、値：ID
	std::unordered_map<std::string, int32_t> pathToIDMap_;
	// キー：ID、値：テクスチャデータ
	std::vector<std::unique_ptr<ModelData>> objects_;
};


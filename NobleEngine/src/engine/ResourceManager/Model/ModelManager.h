#pragma once
#include <definition/definition.h>
#include <ResourceManager/Model/ModelLoader/ModelLoader.h>
#include <ResourceManager/Model/ModelCreater/ModelCreater.h>
#include <ResourceManager/Model/ModelBank/ModelBank.h>
#include <memory>

/// <summary>
/// モデル管理クラス
/// </summary>
class ModelManager
{
public:
	ModelManager(ID3D12Device2* device);
	~ModelManager();

	ModelLoader* GetModelLoader() const { return loader_.get(); }
	ModelCreater* GetModelCreater() const { return creater_.get(); }
	ModelBank* GetModelBank() const { return bank_.get(); }

private:
	std::unique_ptr<ModelLoader> loader_;
	std::unique_ptr<ModelCreater> creater_;
	std::unique_ptr<ModelBank> bank_;

};


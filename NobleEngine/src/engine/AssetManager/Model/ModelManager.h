#pragma once
#include "ModelLoader/ModelLoader.h"
#include "ModelCreater/ModelCreater.h"
#include "ModelBank/ModelBank.h"
#include <memory>

class DirectXManager;

/// <summary>
/// モデル管理クラス
/// </summary>
class ModelManager
{
public:
	ModelManager(DirectXManager* dxManager);
	~ModelManager();

	ModelLoader* GetModelLoader() const { return loader_.get(); }
	ModelCreater* GetModelCreater() const { return creater_.get(); }
	ModelBank* GetModelBank() const { return bank_.get(); }

private:
	std::unique_ptr<ModelLoader> loader_;
	std::unique_ptr<ModelCreater> creater_;
	std::unique_ptr<ModelBank> bank_;
};


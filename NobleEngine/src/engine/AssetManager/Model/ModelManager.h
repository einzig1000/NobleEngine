#pragma once
#include "ModelLoader/ModelLoader.h"
#include "ModelCreator/ModelCreator.h"
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
	ModelCreator* GetModelCreator() const { return creator_.get(); }
	ModelBank* GetModelBank() const { return bank_.get(); }

private:
	std::unique_ptr<ModelLoader> loader_;
	std::unique_ptr<ModelCreator> creator_;
	std::unique_ptr<ModelBank> bank_;
};


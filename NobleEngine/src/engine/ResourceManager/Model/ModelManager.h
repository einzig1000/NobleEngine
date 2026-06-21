#pragma once
#include <definition/definition.h>
#include <ResourceManager/Model/ModelLoader/ModelLoader.h>
#include <ResourceManager/Model/ModelCreater/ModelCreater.h>
#include <ResourceManager/Model/ModelBank/ModelBank.h>
#include <ResourceManager/Model/ModelEditor/ModelEditor.h>
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
	ModelEditor* GetModelEditor() const { return editor_.get(); }

private:
	std::unique_ptr<ModelLoader> loader_;
	std::unique_ptr<ModelCreater> creater_;
	std::unique_ptr<ModelBank> bank_;
	std::unique_ptr<ModelEditor> editor_;

};


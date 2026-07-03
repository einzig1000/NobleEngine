#include "ModelManager.h"
#include <DirectX/DirectXManager.h>

ModelManager::ModelManager(DirectXManager* dxManager)
{
	// モデルバンク作成
	bank_ = std::make_unique<ModelBank>();
	// モデルローダー作成
	loader_ = std::make_unique<ModelLoader>(dxManager, bank_.get());
	// モデルクリエイター作成
	creater_ = std::make_unique<ModelCreater>(dxManager, bank_.get());
}

ModelManager::~ModelManager()
{}

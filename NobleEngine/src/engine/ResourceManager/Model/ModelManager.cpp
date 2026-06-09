#include "ModelManager.h"
#include <Utilities/Logger/Logger.h>
#include <DirectX/Resource/Dx12ResourceFactory.h>
#include <filesystem> 
#include <fstream>


ModelManager::ModelManager(ID3D12Device* device)
{
	// モデルバンク作成
	bank_ = std::make_unique<ModelBank>();
	// モデルローダー作成
	loader_ = std::make_unique<ModelLoader>(device, bank_.get());
	// モデルクリエイター作成
	creater_ = std::make_unique<ModelCreater>(device, bank_.get());
}

ModelManager::~ModelManager()
{}

#include "ModelManager.h"
#include <Utilities/Logger/Logger.h>

ModelManager::ModelManager(DirectXManager* dxManager)
{
	Logger::Log("コンストラクタ実行開始 : ModelManager");

	// モデルバンク作成
	bank_ = std::make_unique<ModelBank>();
	// モデルローダー作成
	loader_ = std::make_unique<ModelLoader>(dxManager, bank_.get());
	// モデルクリエイター作成
	creator_ = std::make_unique<ModelCreator>(dxManager, bank_.get());

	Logger::Log("成功");
}

ModelManager::~ModelManager()
{
	Logger::Log("デストラクタ実行成功 : ModelManager");
}

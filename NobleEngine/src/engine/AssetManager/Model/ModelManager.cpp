#include "ModelManager.h"
#include <Utilities/Logger/Logger.h>

ModelManager::ModelManager(DirectXManager* dxManager)
{
	Log("コンストラクタ実行開始 : ModelManager");

	// モデルバンク作成
	bank_ = std::make_unique<ModelBank>();
	// モデルローダー作成
	loader_ = std::make_unique<ModelLoader>(dxManager, bank_.get());
	// モデルクリエイター作成
	creater_ = std::make_unique<ModelCreater>(dxManager, bank_.get());

	Log("成功");
}

ModelManager::~ModelManager()
{
	Log("デストラクタ実行成功 : ModelManager");
}

#include "AnimationManager.h"
#include <Utilities/Logger/Logger.h>

AnimationManager::AnimationManager(DirectXManager* dxManager)
{
	Logger::Log("コンストラクタ実行開始 : AnimationManager");

	bank_ = std::make_unique<AnimationBank>();
	loader_ = std::make_unique<AnimationLoader>(dxManager, bank_.get());
	computer_ = std::make_unique<AnimationComputer>(bank_.get());

	Logger::Log("成功");
}

AnimationManager::~AnimationManager()
{
	Logger::Log("デストラクタ実行成功 : AnimationManager");
}

#include "RootBindingManager.h"

RootBindingManager::RootBindingManager(DirectXManager* dxManager)
{
	sbManager_ = std::make_unique<StructuredBufferManager>(dxManager);
	cbManager_ = std::make_unique<ConstantBufferManager>(dxManager);
}

RootBindingManager::~RootBindingManager()
{}

void RootBindingManager::Reset()
{
	// このフレームで使うBufferをリセットする
	cbManager_->Reset();
	// 解放待ちのBufferを解放する
	sbManager_->ProcessPendingReleases();
}


#include "RootBindingManager.h"

RootBindingManager::RootBindingManager(ID3D12Device2* device, SwapChainManager* swapChainManager, CommandContextManager* cmdContextManager, SRV_UAVManager* srvUavManager, SynchronizationManager* syncManager)
{
	sbManager_ = std::make_unique<StructuredBufferManager>(device, swapChainManager, cmdContextManager, srvUavManager, syncManager);
	cbManager_ = std::make_unique<ConstantBufferManager>(device, swapChainManager);
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


#pragma once
#include "ConstantBuffer/ConstantBufferManager.h"
#include "StructuredBuffer/StructuredBufferManager.h"

class RootBindingManager
{
public:
	RootBindingManager(ID3D12Device2* device, SwapChainManager* swapChainManager, CommandContextManager* cmdContextManager, SRV_UAVManager* srvUavManager, SynchronizationManager* syncManager);
	~RootBindingManager();
	void Reset();
	ConstantBufferManager* GetConstantBufferManager() { return cbManager_.get(); }
	StructuredBufferManager* GetStructuredBufferManager() { return sbManager_.get(); }


private:
	std::unique_ptr<ConstantBufferManager> cbManager_;
	std::unique_ptr<StructuredBufferManager> sbManager_;
};


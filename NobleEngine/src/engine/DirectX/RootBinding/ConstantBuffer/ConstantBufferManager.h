#pragma once
#include <EngineDefinition/EngineConstexprs.h>
#include "FrameCbAllocator.h"

class SwapChainManager;

class ConstantBufferManager
{
public:
	ConstantBufferManager(ID3D12Device2* device, SwapChainManager* swapChainManager);
	~ConstantBufferManager();
	void Reset();
	void Update();

	D3D12_GPU_VIRTUAL_ADDRESS GetCurrentFrameCbGpuAddress(size_t sizeBytes, const void* data);

private:
	SwapChainManager* swapChainManager_ = nullptr;
	UINT backBufferIndex_ = 0;

	FrameCbAllocator cbAllocators_[Constexprs::kFrameCount]{};
};


#pragma once
#include <EngineDefinition/EngineConstexprs.h>
#include <RootBinding/ConstantBuffer/FrameCbAllocator.h>

class DirectXManager;

class ConstantBufferManager
{
public:
	ConstantBufferManager(DirectXManager* dxManager);
	~ConstantBufferManager();
	void Reset();
	void Update();

	D3D12_GPU_VIRTUAL_ADDRESS GetCurrentFrameCbGpuAddress(size_t sizeBytes, const void* data);

private:
	DirectXManager* dxManager_ = nullptr;
	UINT backBufferIndex_ = 0;

	FrameCbAllocator cbAllocators_[Constexprs::kFrameCount]{};
};


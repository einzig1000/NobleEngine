#include "ConstantBufferManager.h"
#include <DirectX/DirectXManager.h>

ConstantBufferManager::ConstantBufferManager(DirectXManager* dxManager)
	: dxManager_(dxManager)
{
	for (uint32_t i = 0; i < Constexprs::kFrameCount; ++i)
	{
		cbAllocators_[i].Initialize(dxManager_->GetDevice(), 8 * 1024 * 1024, L"FrameCBAllocator");
	}
}

ConstantBufferManager::~ConstantBufferManager()
{}

void ConstantBufferManager::Reset()
{
	backBufferIndex_ = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	// CBアロケータをリセット
	cbAllocators_[backBufferIndex_].Reset();
}

void ConstantBufferManager::Update()
{
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBufferManager::GetCurrentFrameCbGpuAddress(size_t sizeBytes, const void* data)
{
	const auto alloc = cbAllocators_[backBufferIndex_].Allocate(sizeBytes);
	std::memcpy(alloc.cpu, data, static_cast<size_t>(sizeBytes));
	return alloc.gpu;
}

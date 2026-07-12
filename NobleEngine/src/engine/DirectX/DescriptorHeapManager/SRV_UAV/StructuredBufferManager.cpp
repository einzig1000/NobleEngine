#include "StructuredBufferManager.h"
#include <cassert>
#include <cstring>

StructuredBufferManager::StructuredBufferManager(DirectXManager* dxManager)
	: dxManager_(dxManager)
{}

SRV_UAVManager::Allocation StructuredBufferManager::GetStaticSRV(StaticHandle handle) const
{
	assert(handle.index >= 0 && static_cast<size_t>(handle.index) < staticBuffers_.size());
	return staticBuffers_[handle.index].srv;
}

StructuredBufferManager::DynamicHandle StructuredBufferManager::CreateDynamic()
{
	dynamicBuffers_.push_back(DynamicEntry{});
	return DynamicHandle{ static_cast<int32_t>(dynamicBuffers_.size() - 1) };
}

void StructuredBufferManager::UpdateDynamic(DynamicHandle handle, const void* data, size_t elementSize, size_t elementCount)
{
	assert(handle.index >= 0 && static_cast<size_t>(handle.index) < dynamicBuffers_.size());
	auto& entry = dynamicBuffers_[handle.index];

	const size_t bytes = elementSize * elementCount;
	const uint32_t frameIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();

	auto& alloc = entry.srvAllocations[frameIndex];
	bool needsNewBuffer = (entry.buffers[frameIndex] == nullptr) ||
		(bytes > entry.buffers[frameIndex]->GetDesc().Width);

	if (needsNewBuffer)
	{
		entry.buffers[frameIndex] = Dx12ResourceFactory::CreateBufferResource(dxManager_->GetDevice(), bytes);
		entry.buffers[frameIndex]->Map(0, nullptr, &entry.mapped[frameIndex]);
	}

	auto* srvUavManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();
	if (alloc.index == UINT32_MAX)
	{
		alloc = srvUavManager->CreateSRVforStructuredBuffer(
			entry.buffers[frameIndex].Get(), static_cast<UINT>(elementCount), static_cast<UINT>(elementSize));
	}
	else
	{
		srvUavManager->RewriteSRVforStructuredBuffer(
			alloc, entry.buffers[frameIndex].Get(),
			static_cast<UINT>(elementCount), static_cast<UINT>(elementSize));
	}

	assert(entry.mapped[frameIndex]);
	std::memcpy(entry.mapped[frameIndex], data, bytes);
}

SRV_UAVManager::Allocation StructuredBufferManager::GetDynamicSRV(DynamicHandle handle) const
{
	assert(handle.index >= 0 && static_cast<size_t>(handle.index) < dynamicBuffers_.size());
	const uint32_t frameIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	return dynamicBuffers_[handle.index].srvAllocations[frameIndex];
}

StructuredBufferManager::ComputeOutHandle StructuredBufferManager::CreateComputeOutput(size_t elementSize, size_t elementCount)
{
	ComputeOutEntry entry{};
	const size_t bytes = elementSize * elementCount;

	// Defaultヒープ
	entry.buffer = Dx12ResourceFactory::CreateDefaultBufferResource(dxManager_->GetDevice(), bytes);

	// 同じバッファに対して UAVとSRVの両方を作る
	entry.uav = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateUAVforStructuredBuffer(
		entry.buffer.Get(), static_cast<UINT>(elementCount), static_cast<UINT>(elementSize));
	entry.srv = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateSRVforStructuredBuffer(
		entry.buffer.Get(), static_cast<UINT>(elementCount), static_cast<UINT>(elementSize));

	computeOutBuffers_.push_back(entry);
	return ComputeOutHandle{ static_cast<int32_t>(computeOutBuffers_.size() - 1) };
}

SRV_UAVManager::Allocation StructuredBufferManager::GetComputeOutputUAV(ComputeOutHandle handle) const
{
	assert(handle.index >= 0 && static_cast<size_t>(handle.index) < computeOutBuffers_.size());
	return computeOutBuffers_[handle.index].uav;
}

SRV_UAVManager::Allocation StructuredBufferManager::GetComputeOutputSRV(ComputeOutHandle handle) const
{
	assert(handle.index >= 0 && static_cast<size_t>(handle.index) < computeOutBuffers_.size());
	return computeOutBuffers_[handle.index].srv;
}

ID3D12Resource* StructuredBufferManager::GetComputeOutputResource(ComputeOutHandle handle) const
{
	assert(handle.index >= 0 && static_cast<size_t>(handle.index) < computeOutBuffers_.size());
	return computeOutBuffers_[handle.index].buffer.Get();
}

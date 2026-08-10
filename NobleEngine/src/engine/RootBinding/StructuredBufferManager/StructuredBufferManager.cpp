#include "StructuredBufferManager.h"

StructuredBufferManager::StructuredBufferManager(DirectXManager* dxManager)
	: dxManager_(dxManager)
{}


int32_t StructuredBufferManager::CreateDynamic()
{
	dynamicBuffers_[nextHandleIndex_] = DynamicEntry{};
	kindMap_[nextHandleIndex_] = BufferKind::Dynamic;
	return nextHandleIndex_++;
}

int32_t StructuredBufferManager::CreateComputeOutput(size_t elementSize, size_t elementCount)
{
	ComputeOutEntry entry{};
	const size_t bytes = elementSize * elementCount;

	// デフォルトヒープ(GPUからの高速アクセス)に作る
	entry.buffer = Dx12ResourceFactory::CreateDefaultBufferResource(dxManager_->GetDevice(), bytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	// 同じバッファに対して UAVとSRVの両方を作る
	entry.uav = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateUAVforStructuredBuffer(
		entry.buffer.Get(), static_cast<UINT>(elementCount), static_cast<UINT>(elementSize));
	entry.srv = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateSRVforStructuredBuffer(
		entry.buffer.Get(), static_cast<UINT>(elementCount), static_cast<UINT>(elementSize));

	computeOutBuffers_[nextHandleIndex_] = std::move(entry);
	kindMap_[nextHandleIndex_] = BufferKind::ComputeOutput;
	return nextHandleIndex_++;
}



void StructuredBufferManager::UpdateData(int32_t handle, const void* data, size_t elementSize, size_t elementCount)
{
	if (kindMap_.find(handle) == kindMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferを更新しようとしました。");
		return;
	}
	
	if (kindMap_.at(handle) != BufferKind::Dynamic)
	{
		Log("Dynamicのバッファ以外はUpdateDataできません");
		return;
	}

	auto& entry = dynamicBuffers_[handle];
	const size_t bytes = elementSize * elementCount;
	const uint32_t frameIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	auto* srvUav = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();

	// resourceが(そもそも作成されてない || サイズが足りない)なら作る
	if ((entry.buffers[frameIndex] == nullptr) || (bytes > entry.buffers[frameIndex]->GetDesc().Width))
	{
		entry.buffers[frameIndex] = Dx12ResourceFactory::CreateBufferResource(dxManager_->GetDevice(), bytes);
 		entry.buffers[frameIndex]->Map(0, nullptr, &entry.mapped[frameIndex]);
	}

	// srvが作成されていないなら作る
	if (entry.srvAllocations[frameIndex].index == UINT32_MAX)
	{
		entry.srvAllocations[frameIndex] = srvUav->CreateSRVforStructuredBuffer(
			entry.buffers[frameIndex].Get(), static_cast<UINT>(elementCount), static_cast<UINT>(elementSize));
	}
	// SRVが既にあるなら更新する
	else
	{
		srvUav->RewriteSRVforStructuredBuffer(
			entry.srvAllocations[frameIndex], entry.buffers[frameIndex].Get(),
			static_cast<UINT>(elementCount), static_cast<UINT>(elementSize));
	}

	// データをコピーする
	assert(entry.mapped[frameIndex]);
	std::memcpy(entry.mapped[frameIndex], data, bytes);
}



void StructuredBufferManager::Destroy(int32_t handle)
{
	(void)handle;
}



uint32_t StructuredBufferManager::GetSRV(int32_t handle) const
{
	auto it = kindMap_.find(handle);
	if (it == kindMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferのSRVを取得しようとしました。");
		return UINT32_MAX;
	}

	switch (it->second)
	{
	case BufferKind::Static:        return staticBuffers_.at(handle).srv.index;
	case BufferKind::Dynamic:       return dynamicBuffers_.at(handle).srvAllocations[dxManager_->GetSwapChain()->GetCurrentBackBufferIndex()].index;
	case BufferKind::ComputeOutput: return computeOutBuffers_.at(handle).srv.index;
	}

	return UINT32_MAX;
}

uint32_t StructuredBufferManager::GetUAV(int32_t handle) const
{
	auto it = kindMap_.find(handle);
	if (it == kindMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferのUAVを取得しようとしました。");
		return UINT32_MAX;
	}

	if (it->second != BufferKind::ComputeOutput)
	{
		Log("ComputeOutputのバッファ以外はUAVを取得できません");
		return UINT32_MAX;
	}

	return computeOutBuffers_.at(handle).uav.index;
}

ID3D12Resource* StructuredBufferManager::GetResource(int32_t handle) const
{
	if (kindMap_.find(handle) == kindMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferのResourceを取得しようとしました。");
		return nullptr;
	}

	switch (kindMap_.at(handle))
	{
	case BufferKind::Static:        return staticBuffers_.at(handle).buffer.Get();
	case BufferKind::Dynamic:       return dynamicBuffers_.at(handle).buffers[dxManager_->GetSwapChain()->GetCurrentBackBufferIndex()].Get();
	case BufferKind::ComputeOutput: return computeOutBuffers_.at(handle).buffer.Get();
	}

	return nullptr;
}



void StructuredBufferManager::TransitionToUAV(int32_t handle, ID3D12GraphicsCommandList6* cmdList)
{
	if (kindMap_.find(handle) == kindMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferをUAVに遷移しようとしました。");
		return;
	}

	if (kindMap_.at(handle) != BufferKind::ComputeOutput)
	{
		Log("ComputeOutputのバッファ以外はUAVに遷移できません");
		return;
	}

	auto& entry = computeOutBuffers_.at(handle);

	// 既にUAV状態ならreturn
	if (entry.currentState == D3D12_RESOURCE_STATE_UNORDERED_ACCESS) return;

	Dx12ResourceTransition::Transition(cmdList, entry.buffer.Get(), entry.currentState, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	entry.currentState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
}

void StructuredBufferManager::TransitionToSRV(int32_t handle, ID3D12GraphicsCommandList6* cmdList)
{
	if (kindMap_.find(handle) == kindMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferをSRVに遷移しようとしました。");
		return;
	}

	if (kindMap_.at(handle) != BufferKind::ComputeOutput)
	{
		Log("ComputeOutputのバッファ以外はSRVに遷移できません");
		return;
	}

	auto& entry = computeOutBuffers_.at(handle);

	// VS・PSどちらから読めるようにするため両方のシェーダーステージフラグを立てる
	constexpr D3D12_RESOURCE_STATES kSRVState =
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	if (entry.currentState == kSRVState) return;

	Dx12ResourceTransition::Transition(cmdList, entry.buffer.Get(), entry.currentState, kSRVState);
	entry.currentState = kSRVState;
}


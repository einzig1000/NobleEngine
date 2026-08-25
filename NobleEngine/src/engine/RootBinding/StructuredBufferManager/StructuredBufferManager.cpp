#include "StructuredBufferManager.h"
#include <Utilities/Logger/Logger.h>

StructuredBufferManager::StructuredBufferManager(DirectXManager* dxManager)
	: dxManager_(dxManager)
{}


int32_t StructuredBufferManager::CreateDynamic()
{
	dynamicBuffers_[nextResourceID_] = DynamicEntry{};
	kindMap_[nextResourceID_] = BufferKind::Dynamic;
	return nextResourceID_++;
}

int32_t StructuredBufferManager::CreateCompute(size_t elementSize, size_t elementCount)
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

	computeOutBuffers_[nextResourceID_] = std::move(entry);
	kindMap_[nextResourceID_] = BufferKind::ComputeOutput;
	return nextResourceID_++;
}



void StructuredBufferManager::UpdateData(int32_t resourceID, const void* data, size_t elementSize, size_t elementCount)
{
	if (kindMap_.find(resourceID) == kindMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferを更新しようとしました。");
		return;
	}
	
	if (kindMap_.at(resourceID) != BufferKind::Dynamic)
	{
		Log("Dynamicのバッファ以外はUpdateDataできません");
		return;
	}

	auto& entry = dynamicBuffers_[resourceID];
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

void StructuredBufferManager::ZeroFillCompute(int32_t resourceID, size_t bytes)
{
	if (kindMap_.find(resourceID) == kindMap_.end())
	{
		Log("存在しないハンドルのComputeOutputバッファをゼロクリアしようとしました。");
		return;
	}
	if (kindMap_.at(resourceID) != BufferKind::ComputeOutput)
	{
		Log("ComputeOutput以外のバッファはZeroFillComputeOutputできません");
		return;
	}

	auto& entry = computeOutBuffers_.at(resourceID);
	const auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);

	std::vector<uint8_t> zeros(bytes, 0);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediate =
		Dx12ResourceFactory::CreateBufferResource(dxManager_->GetDevice(), bytes);
	void* mapped = nullptr;
	intermediate->Map(0, nullptr, &mapped);
	std::memcpy(mapped, zeros.data(), bytes);
	intermediate->Unmap(0, nullptr);
	pendingIntermediates_.push_back(intermediate);

	D3D12_RESOURCE_STATES currentState = entry.currentState;
	Dx12ResourceTransition::Transition(cmdList, entry.buffer.Get(), currentState, D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyBufferRegion(entry.buffer.Get(), 0, intermediate.Get(), 0, bytes);
	Dx12ResourceTransition::Transition(cmdList, entry.buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	entry.currentState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
}



void StructuredBufferManager::Destroy(int32_t resourceID)
{
	(void)resourceID;
}



uint32_t StructuredBufferManager::GetSRV(int32_t resourceID) const
{
	auto it = kindMap_.find(resourceID);
	if (it == kindMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferのSRVを取得しようとしました。");
		__debugbreak();
		return UINT32_MAX;
	}

	switch (it->second)
	{
	case BufferKind::Static:        return staticBuffers_.at(resourceID).srv.index;
	case BufferKind::Dynamic:       return dynamicBuffers_.at(resourceID).srvAllocations[dxManager_->GetSwapChain()->GetCurrentBackBufferIndex()].index;
	case BufferKind::ComputeOutput: return computeOutBuffers_.at(resourceID).srv.index;
	}

	__debugbreak();
	return UINT32_MAX;
}

uint32_t StructuredBufferManager::GetUAV(int32_t resourceID) const
{
	auto it = kindMap_.find(resourceID);
	if (it == kindMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferのUAVを取得しようとしました。");
		__debugbreak();
		return UINT32_MAX;
	}

	if (it->second != BufferKind::ComputeOutput)
	{
		Log("ComputeOutputのバッファ以外はUAVを取得できません");
		__debugbreak();
		return UINT32_MAX;
	}

	return computeOutBuffers_.at(resourceID).uav.index;
}

ID3D12Resource* StructuredBufferManager::GetResource(int32_t resourceID) const
{
	if (kindMap_.find(resourceID) == kindMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferのResourceを取得しようとしました。");
		return nullptr;
	}

	switch (kindMap_.at(resourceID))
	{
	case BufferKind::Static:        return staticBuffers_.at(resourceID).buffer.Get();
	case BufferKind::Dynamic:       return dynamicBuffers_.at(resourceID).buffers[dxManager_->GetSwapChain()->GetCurrentBackBufferIndex()].Get();
	case BufferKind::ComputeOutput: return computeOutBuffers_.at(resourceID).buffer.Get();
	}

	return nullptr;
}



int32_t StructuredBufferManager::RequestReadback(int32_t resourceID, size_t bytes)
{
	if (kindMap_.find(resourceID) == kindMap_.end())
	{
		Log("存在しないハンドルの読み戻しを要求しようとしました。");
		return -1;
	}
	if (kindMap_.at(resourceID) != BufferKind::ComputeOutput)
	{
		Log("ComputeOutput以外のバッファは読み戻しできません");
		return -1;
	}

	int32_t token = nextReadbackToken_++;
	PendingReadback pending{};
	pending.sourceResourceID = resourceID;
	pending.bytes = bytes;
	pendingReadbacks_[token] = pending;
	return token;
}

void StructuredBufferManager::FlushPendingReadbackRequests()
{
	if (pendingReadbacks_.empty()) return;

	const auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);
	const UINT64 targetFenceValue = dxManager_->GetSynchronizationManager()->GetNextFenceValue();

	for (auto& [token, pending] : pendingReadbacks_)
	{
		if (pending.copyRecorded) continue; // 結果待ちで複数フレームまたいでいるものはスキップ

		auto& entry = computeOutBuffers_.at(pending.sourceResourceID);

		pending.readbackResource = Dx12ResourceFactory::CreateReadbackResource(dxManager_->GetDevice(), pending.bytes);

		D3D12_RESOURCE_STATES originalState = entry.currentState;
		Dx12ResourceTransition::Transition(cmdList, entry.buffer.Get(), originalState, D3D12_RESOURCE_STATE_COPY_SOURCE);
		cmdList->CopyBufferRegion(pending.readbackResource.Get(), 0, entry.buffer.Get(), 0, pending.bytes);
		Dx12ResourceTransition::Transition(cmdList, entry.buffer.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, originalState);
		entry.currentState = originalState;

		pending.targetFenceValue = targetFenceValue;
		pending.copyRecorded = true;
	}
}

bool StructuredBufferManager::TryGetReadbackResult(int32_t readbackToken, void* outData, size_t bytes)
{
	auto it = pendingReadbacks_.find(readbackToken);
	if (it == pendingReadbacks_.end())
	{
		return false;
	}

	PendingReadback& pending = it->second;
	if (!pending.copyRecorded) return false; // まだこのフレームのFlushが呼ばれていない

	if (!dxManager_->GetSynchronizationManager()->IsFenceValueReached(pending.targetFenceValue))
	{
		return false; // まだGPU側の処理が終わっていない
	}

	void* mapped = nullptr;
	D3D12_RANGE readRange{ 0, pending.bytes };
	HRESULT hr = pending.readbackResource->Map(0, &readRange, &mapped);
	if (FAILED(hr))
	{
		Log("読み戻しバッファのMapに失敗しました。");
		pendingReadbacks_.erase(it);
		return false;
	}

	std::memcpy(outData, mapped, std::min(bytes, pending.bytes));

	D3D12_RANGE writtenRange{ 0, 0 }; // CPU側は書き込んでいない
	pending.readbackResource->Unmap(0, &writtenRange);

	pendingReadbacks_.erase(it);
	return true;
}


void StructuredBufferManager::TransitionToUAV(int32_t resourceID, ID3D12GraphicsCommandList6* cmdList)
{
	if (kindMap_.find(resourceID) == kindMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferをUAVに遷移しようとしました。");
		return;
	}

	if (kindMap_.at(resourceID) != BufferKind::ComputeOutput)
	{
		Log("ComputeOutputのバッファ以外はUAVに遷移できません");
		return;
	}

	auto& entry = computeOutBuffers_.at(resourceID);

	// 既にUAV状態ならreturn
	if (entry.currentState == D3D12_RESOURCE_STATE_UNORDERED_ACCESS) return;

	Dx12ResourceTransition::Transition(cmdList, entry.buffer.Get(), entry.currentState, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	entry.currentState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
}

void StructuredBufferManager::TransitionToSRV(int32_t resourceID, ID3D12GraphicsCommandList6* cmdList)
{
	if (kindMap_.find(resourceID) == kindMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferをSRVに遷移しようとしました。");
		return;
	}

	if (kindMap_.at(resourceID) != BufferKind::ComputeOutput)
	{
		Log("ComputeOutputのバッファ以外はSRVに遷移できません");
		return;
	}

	auto& entry = computeOutBuffers_.at(resourceID);

	// VS・PSどちらから読めるようにするため両方のシェーダーステージフラグを立てる
	constexpr D3D12_RESOURCE_STATES kSRVState =
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	if (entry.currentState == kSRVState) return;

	Dx12ResourceTransition::Transition(cmdList, entry.buffer.Get(), entry.currentState, kSRVState);
	entry.currentState = kSRVState;
}


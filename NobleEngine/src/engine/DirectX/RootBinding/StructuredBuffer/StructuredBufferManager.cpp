#include "StructuredBufferManager.h"
#include <Utilities/Logger/Logger.h>
#include <DirectX/DeviceManager/DeviceManager.h>
#include <DirectX/CommandContextManager/CommandContextManager.h>
#include <DirectX/RenderTarget/SwapChainManager/SwapChainManager.h>
#include <DirectX/SynchronizationManager/SynchronizationManager.h>
#include <DirectX/DescriptorHeapManager/SRV_UAV/SRV_UAVManager.h>

StructuredBufferManager::StructuredBufferManager(ID3D12Device2* device, SwapChainManager* swapChainManager, CommandContextManager* cmdContextManager, SRV_UAVManager* srvUavManager, SynchronizationManager* syncManager)
	: device_(device), swapChainManager_(swapChainManager), cmdContextManager_(cmdContextManager), srvUavManager_(srvUavManager), syncManager_(syncManager)
{}


int32_t StructuredBufferManager::CreateStatic(const void* data, size_t elementSize, size_t elementCount)
{
	if (data == nullptr || elementSize == 0 || elementCount == 0)
	{
		Log("StructuredBufferManager::CreateStatic() dataがnullptr、またはelementSize/elementCountが0です");
		return -1;
	}

	// このフレームで使うコマンドリストを取得
	const auto backBufferIndex = swapChainManager_->GetCurrentBackBufferIndex();
	auto* cmdList = cmdContextManager_->GetCommandList(backBufferIndex);

	StaticEntry entry{};
	const size_t bytes = elementSize * elementCount;

	// デフォルトヒープ(GPUからの高速アクセス)に作る
	entry.buffer = Dx12ResourceFactory::CreateDefaultBufferResource(device_, bytes);

	// デフォルトヒープに送るためにこのフレームでだけ使いたいアップロードヒープを作る
	auto intermediate = Dx12ResourceFactory::CreateUploadResource(entry.buffer.Get(), data, bytes, device_, cmdList);
	pendingIntermediates_.push_back(intermediate);

	// SRVを作る
	entry.srv = srvUavManager_->CreateSRVforStructuredBuffer(
		entry.buffer.Get(), static_cast<UINT>(elementCount), static_cast<UINT>(elementSize));

	staticBuffers_[nextResourceID_] = std::move(entry);
	bufferTypeMap_[nextResourceID_] = BufferType::Static;
	return nextResourceID_++;
}

int32_t StructuredBufferManager::CreateDynamic()
{
	dynamicBuffers_[nextResourceID_] = DynamicEntry{};
	bufferTypeMap_[nextResourceID_] = BufferType::Dynamic;
	return nextResourceID_++;
}

int32_t StructuredBufferManager::CreateCompute(size_t elementSize, size_t elementCount)
{
	if (elementSize == 0 || elementCount == 0)
	{
		Log("StructuredBufferManager::CreateCompute() elementSize/elementCountが0です");
		return -1;
	}

	ComputeOutEntry entry{};
	const size_t bytes = elementSize * elementCount;

	// デフォルトヒープ(GPUからの高速アクセス)に作る
	entry.buffer = Dx12ResourceFactory::CreateDefaultBufferResource(device_, bytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	// 同じバッファに対して UAVとSRVの両方を作る
	entry.uav = srvUavManager_->CreateUAVforStructuredBuffer(
		entry.buffer.Get(), static_cast<UINT>(elementCount), static_cast<UINT>(elementSize));
	entry.srv = srvUavManager_->CreateSRVforStructuredBuffer(
		entry.buffer.Get(), static_cast<UINT>(elementCount), static_cast<UINT>(elementSize));

	computeOutBuffers_[nextResourceID_] = std::move(entry);
	bufferTypeMap_[nextResourceID_] = BufferType::ComputeOutput;
	return nextResourceID_++;
}



void StructuredBufferManager::UpdateData(int32_t resourceID, const void* data, size_t elementSize, size_t elementCount)
{
	if (bufferTypeMap_.find(resourceID) == bufferTypeMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferを更新しようとしました。");
		return;
	}
	
	if (bufferTypeMap_.at(resourceID) != BufferType::Dynamic)
	{
		Log("Dynamicのバッファ以外はUpdateDataできません");
		return;
	}

	auto& entry = dynamicBuffers_[resourceID];
	const size_t bytes = elementSize * elementCount;
	const uint32_t frameIndex = swapChainManager_->GetCurrentBackBufferIndex();
	auto* srvUav = srvUavManager_;

	// resourceが(そもそも作成されてない || サイズが足りない)なら作る
	if ((entry.buffers[frameIndex] == nullptr) || (bytes > entry.buffers[frameIndex]->GetDesc().Width))
	{
		entry.buffers[frameIndex] = Dx12ResourceFactory::CreateBufferResource(device_, bytes);
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
	if (bufferTypeMap_.find(resourceID) == bufferTypeMap_.end())
	{
		Log("存在しないハンドルのComputeOutputバッファをゼロクリアしようとしました。");
		return;
	}
	if (bufferTypeMap_.at(resourceID) != BufferType::ComputeOutput)
	{
		Log("ComputeOutput以外のバッファはZeroFillComputeOutputできません");
		return;
	}

	auto& entry = computeOutBuffers_.at(resourceID);
	const auto backBufferIndex = swapChainManager_->GetCurrentBackBufferIndex();
	auto* cmdList = cmdContextManager_->GetCommandList(backBufferIndex);

	std::vector<uint8_t> zeros(bytes, 0);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediate =
		Dx12ResourceFactory::CreateBufferResource(device_, bytes);
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
	auto type = bufferTypeMap_.find(resourceID);
	if (type == bufferTypeMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferを解放しようとしました。");
		return;
	}

	PendingRelease release{};
	// このフレームのコマンドが完了した時点で解放してよい
	release.fenceValue = syncManager_->GetNextFenceValue();

	switch (type->second)
	{
	case BufferType::Static:
	{
		auto& entry = staticBuffers_.at(resourceID);
		release.resources.push_back(std::move(entry.buffer));
		release.descriptorIndices.push_back(entry.srv.index);
		staticBuffers_.erase(resourceID);
		break;
	}
	case BufferType::Dynamic:
	{
		auto& entry = dynamicBuffers_.at(resourceID);
		for (uint32_t i = 0; i < Constexprs::kFrameCount; ++i)
		{
			if (entry.buffers[i]) { release.resources.push_back(std::move(entry.buffers[i])); }
			if (entry.srvAllocations[i].index != UINT32_MAX) { release.descriptorIndices.push_back(entry.srvAllocations[i].index); }
		}
		dynamicBuffers_.erase(resourceID);
		break;
	}
	case BufferType::ComputeOutput:
	{
		auto& entry = computeOutBuffers_.at(resourceID);
		release.resources.push_back(std::move(entry.buffer));
		release.descriptorIndices.push_back(entry.uav.index);
		release.descriptorIndices.push_back(entry.srv.index);
		computeOutBuffers_.erase(resourceID);
		break;
	}
	}
	bufferTypeMap_.erase(type);

	// このリソースを読み戻し中なら読み戻しも破棄する
	for (auto it = pendingReadbacks_.begin(); it != pendingReadbacks_.end();)
	{
		if (it->second.sourceResourceID == resourceID)
		{
			if (it->second.readbackResource) { release.resources.push_back(std::move(it->second.readbackResource)); }
			it = pendingReadbacks_.erase(it);
		}
		else
		{
			++it;
		}
	}

	pendingReleases_.push_back(std::move(release));
}

void StructuredBufferManager::ProcessPendingReleases()
{
	for (auto it = pendingReleases_.begin(); it != pendingReleases_.end();)
	{
		if (syncManager_->IsFenceValueReached(it->fenceValue))
		{
			for (uint32_t index : it->descriptorIndices)
			{
				srvUavManager_->Free(index);
			}
			it = pendingReleases_.erase(it);
		}
		else
		{
			++it;
		}
	}
}



uint32_t StructuredBufferManager::GetSRV(int32_t resourceID) const
{
	auto it = bufferTypeMap_.find(resourceID);
	if (it == bufferTypeMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferのSRVを取得しようとしました。");
		__debugbreak();
		return UINT32_MAX;
	}

	switch (it->second)
	{
	case BufferType::Static:        return staticBuffers_.at(resourceID).srv.index;
	case BufferType::Dynamic:       return dynamicBuffers_.at(resourceID).srvAllocations[swapChainManager_->GetCurrentBackBufferIndex()].index;
	case BufferType::ComputeOutput: return computeOutBuffers_.at(resourceID).srv.index;
	}

	__debugbreak();
	return UINT32_MAX;
}

uint32_t StructuredBufferManager::GetUAV(int32_t resourceID) const
{
	auto it = bufferTypeMap_.find(resourceID);
	if (it == bufferTypeMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferのUAVを取得しようとしました。");
		__debugbreak();
		return UINT32_MAX;
	}

	if (it->second != BufferType::ComputeOutput)
	{
		Log("ComputeOutputのバッファ以外はUAVを取得できません");
		__debugbreak();
		return UINT32_MAX;
	}

	return computeOutBuffers_.at(resourceID).uav.index;
}

ID3D12Resource* StructuredBufferManager::GetResource(int32_t resourceID) const
{
	if (bufferTypeMap_.find(resourceID) == bufferTypeMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferのResourceを取得しようとしました。");
		return nullptr;
	}

	switch (bufferTypeMap_.at(resourceID))
	{
	case BufferType::Static:        return staticBuffers_.at(resourceID).buffer.Get();
	case BufferType::Dynamic:       return dynamicBuffers_.at(resourceID).buffers[swapChainManager_->GetCurrentBackBufferIndex()].Get();
	case BufferType::ComputeOutput: return computeOutBuffers_.at(resourceID).buffer.Get();
	}

	return nullptr;
}



int32_t StructuredBufferManager::RequestReadback(int32_t resourceID, size_t bytes)
{
	if (bufferTypeMap_.find(resourceID) == bufferTypeMap_.end())
	{
		Log("存在しないハンドルの読み戻しを要求しようとしました。");
		return -1;
	}
	if (bufferTypeMap_.at(resourceID) != BufferType::ComputeOutput)
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

	const auto backBufferIndex = swapChainManager_->GetCurrentBackBufferIndex();
	auto* cmdList = cmdContextManager_->GetCommandList(backBufferIndex);
	const UINT64 targetFenceValue = syncManager_->GetNextFenceValue();

	for (auto& [token, pending] : pendingReadbacks_)
	{
		if (pending.copyRecorded) continue; // 結果待ちで複数フレームまたいでいるものはスキップ

		auto& entry = computeOutBuffers_.at(pending.sourceResourceID);

		pending.readbackResource = Dx12ResourceFactory::CreateReadbackResource(device_, pending.bytes);

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

	if (!syncManager_->IsFenceValueReached(pending.targetFenceValue))
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
	if (bufferTypeMap_.find(resourceID) == bufferTypeMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferをUAVに遷移しようとしました。");
		return;
	}

	if (bufferTypeMap_.at(resourceID) != BufferType::ComputeOutput)
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
	if (bufferTypeMap_.find(resourceID) == bufferTypeMap_.end())
	{
		Log("存在しないハンドルのStructuredBufferをSRVに遷移しようとしました。");
		return;
	}

	if (bufferTypeMap_.at(resourceID) != BufferType::ComputeOutput)
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


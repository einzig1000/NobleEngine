#pragma once
#include <vector>
#include <unordered_map>
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include <EngineDefinition/EngineConstexprs.h>
#include <DirectX/DescriptorHeapManager/SRV_UAV/SRV_UAVManager.h>
#include <DirectX/Resource/Dx12ResourceUtilities.h>
#include <DirectX/DirectXManager.h>

enum class BufferKind
{
	Static,
	Dynamic,
	ComputeOutput
};

class StructuredBufferManager
{
public:
	StructuredBufferManager(DirectXManager* dxManager);

	template <typename T>
	int32_t CreateStatic(const std::vector<T>& data)
	{
		// このフレームで使うコマンドリストを取得
		const auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
		auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);

		StaticEntry entry{};
		const size_t bytes = data.size() * sizeof(T);

		// デフォルトヒープ(GPUからの高速アクセス)に作る
		entry.buffer = Dx12ResourceFactory::CreateDefaultBufferResource(dxManager_->GetDevice(), bytes);

		// デフォルトヒープに送るためにこのフレームでだけ使ういたいアップロードヒープを作る
		auto intermediate = Dx12ResourceFactory::CreateUploadResource(entry.buffer.Get(), data, dxManager_->GetDevice(), cmdList);
		pendingIntermediates_.push_back(intermediate);

		// SRVを作る
		entry.srv = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateSRVforStructuredBuffer(
			entry.buffer.Get(), static_cast<UINT>(data.size()), static_cast<UINT>(sizeof(T)));

		staticBuffers_[nextHandleIndex_] = std::move(entry);
		kindMap_[nextHandleIndex_] = BufferKind::Static;
		return nextHandleIndex_++;
	}
	int32_t CreateDynamic();
	int32_t CreateComputeOutput(size_t elementSize, size_t elementCount);

	void UpdateData(int32_t handle, const void* data, size_t elementSize, size_t elementCount);

	void ZeroFillComputeOutput(int32_t handle, size_t bytes);

	void Destroy(int32_t handle);

	uint32_t GetSRV(int32_t heapSlot) const;
	uint32_t GetUAV(int32_t heapSlot) const; // Compute出力のみ有効
	ID3D12Resource* GetResource(int32_t heapSlot) const; // バリア発行用


	void TransitionToUAV(int32_t heapSlot, ID3D12GraphicsCommandList6* cmdList);
	void TransitionToSRV(int32_t heapSlot, ID3D12GraphicsCommandList6* cmdList);

private:
	DirectXManager* dxManager_ = nullptr;

	struct StaticEntry
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
		SRV_UAVManager::Allocation srv;
	};
	struct DynamicEntry
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> buffers[Constexprs::kFrameCount];
		void* mapped[Constexprs::kFrameCount] = { nullptr };
		SRV_UAVManager::Allocation srvAllocations[Constexprs::kFrameCount];
	};
	struct ComputeOutEntry
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
		SRV_UAVManager::Allocation uav;
		SRV_UAVManager::Allocation srv;
		D3D12_RESOURCE_STATES currentState = D3D12_RESOURCE_STATE_COMMON;
	};

	// いずれフリーインデックス配列を使うようにする
	std::unordered_map<int32_t, StaticEntry> staticBuffers_{};
	std::unordered_map<int32_t, DynamicEntry> dynamicBuffers_{};
	std::unordered_map<int32_t, ComputeOutEntry> computeOutBuffers_{};
	int32_t nextHandleIndex_ = 0;
	std::unordered_map<int32_t, BufferKind> kindMap_{};

	// TextureLoaderにもmodelLoaderにもある中間リソース　いつか統合。毎フレーム解放
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> pendingIntermediates_{};

};



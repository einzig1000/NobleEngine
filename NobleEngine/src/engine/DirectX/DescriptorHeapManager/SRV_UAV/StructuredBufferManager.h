#pragma once
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <EngineDefinition/EngineConstexprs.h>
#include <DirectX/DescriptorHeapManager/SRV_UAV/SRV_UAVManager.h>
#include <DirectX/Resource/Dx12ResourceFactory.h>
#include <DirectX/DirectXManager.h>

class StructuredBufferManager
{
public:
	struct StaticHandle { int32_t index = -1; };
	struct DynamicHandle { int32_t index = -1; };
	struct ComputeOutHandle { int32_t index = -1; };

	StructuredBufferManager(DirectXManager* dxManager);

	// ① 静的：一度だけ作って以後不変
	template <typename T>
	StaticHandle CreateStatic(const std::vector<T>& data)
	{
		StaticEntry entry{};
		const size_t bytes = data.size() * sizeof(T);

		entry.buffer = Dx12ResourceFactory::CreateDefaultBufferResource(dxManager_->GetDevice(), bytes);

		const auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
		auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);

		auto intermediate = Dx12ResourceFactory::CreateUploadResource(entry.buffer.Get(), data, dxManager_->GetDevice(), cmdList);
		pendingIntermediates_.push_back(intermediate);

		entry.srv = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateSRVforStructuredBuffer(
			entry.buffer.Get(), static_cast<UINT>(data.size()), static_cast<UINT>(sizeof(T)));

		staticBuffers_.push_back(entry);
		return StaticHandle{ static_cast<int32_t>(staticBuffers_.size() - 1) };
	}
	SRV_UAVManager::Allocation GetStaticSRV(StaticHandle handle) const;

	// ② 動的：識別子固定、中身は毎フレームCPUが書き換える
	DynamicHandle CreateDynamic();
	void UpdateDynamic(DynamicHandle handle, const void* data, size_t elementSize, size_t elementCount);
	SRV_UAVManager::Allocation GetDynamicSRV(DynamicHandle handle) const;

	// ③ Compute出力：CSが書き込み、VS/PS等が読み取る
	ComputeOutHandle CreateComputeOutput(size_t elementSize, size_t elementCount);
	SRV_UAVManager::Allocation GetComputeOutputUAV(ComputeOutHandle handle) const;
	SRV_UAVManager::Allocation GetComputeOutputSRV(ComputeOutHandle handle) const;
	ID3D12Resource* GetComputeOutputResource(ComputeOutHandle handle) const; // バリア発行用

private:
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
	};

	std::vector<StaticEntry> staticBuffers_{};
	std::vector<DynamicEntry> dynamicBuffers_{};
	std::vector<ComputeOutEntry> computeOutBuffers_{};

	// TexLoaderにもmodelLOaderにもある中間リソース　いつかとうごう。毎フレーム解放
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> pendingIntermediates_{};

	DirectXManager* dxManager_ = nullptr;
};
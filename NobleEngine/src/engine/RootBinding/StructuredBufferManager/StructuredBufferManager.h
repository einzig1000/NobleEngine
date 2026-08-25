#pragma once
#include <vector>
#include <unordered_map>
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include <EngineDefinition/EngineConstexprs.h>
#include <DirectX/DescriptorHeapManager/SRV_UAV/SRV_UAVManager.h>
#include <DirectX/ResourceUtilities/ResourceUtilities.h>
#include <DirectX/DirectXManager.h>

enum class BufferKind
{
	Static,
	Dynamic,
	ComputeOutput
};

struct PendingReadback
{
	int32_t sourceResourceID = -1;
	size_t bytes = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> readbackResource;
	UINT64 targetFenceValue = 0;
	bool copyRecorded = false;
};

class StructuredBufferManager
{
public:
	StructuredBufferManager(DirectXManager* dxManager);

	/// <summary>
	/// 静的リソースの作成(モデルやテクスチャ等)
	/// </summary>
	/// <typeparam name="T">データ型</typeparam>
	/// <param name="data">データ</param>
	/// <returns>リソースID</returns>
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

		staticBuffers_[nextResourceID_] = std::move(entry);
		kindMap_[nextResourceID_] = BufferKind::Static;
		return nextResourceID_++;
	}

	/// <summary>
	/// 動的リソースの作成(毎フレーム変わるパーティクル配列等)
	/// </summary>
	/// <returns>リソースID</returns>
	int32_t CreateDynamic();

	/// <summary>
	/// コンピュートリソースの作成(UAVも作成される)
	/// </summary>
	/// <param name="elementSize">要素のサイズ</param>
	/// <param name="elementCount">要素の数</param>
	/// <returns>リソースID</returns>
	int32_t CreateCompute(size_t elementSize, size_t elementCount);


	/// <summary>
	/// 動的リソースの更新
	/// </summary>
	/// <param name="resourceID">リソースID</param>
	/// <param name="data">更新するデータ</param>
	/// <param name="elementSize">要素のサイズ</param>
	/// <param name="elementCount">要素の数</param>
	void UpdateData(int32_t resourceID, const void* data, size_t elementSize, size_t elementCount);

	/// <summary>
	/// コンピュートリソースの0初期化
	/// </summary>
	/// <param name="resourceID">リソースID</param>
	/// <param name="bytes">初期化するバイト数</param>
	void ZeroFillCompute(int32_t resourceID, size_t bytes);

	/// <summary>
	/// リソースの解放
	/// </summary>
	/// <param name="resourceID">リソースID</param>
	void Destroy(int32_t resourceID);

	/// <summary>
	/// リソースのSRVを取得
	/// </summary>
	/// <param name="resourceID">リソースID</param>
	/// <returns>SRVのヒープスロット</returns>
	uint32_t GetSRV(int32_t resourceID) const;

	/// <summary>
	/// リソースのUAVを取得(コンピュートリソースのみ)
	/// </summary>
	/// <param name="resourceID">リソースID</param>
	/// <returns>UAVのヒープスロット</returns>
	uint32_t GetUAV(int32_t resourceID) const;

	/// <summary>
	/// リソースのポインタを取得
	/// </summary>
	/// <param name="resourceID">リソースID</param>
	/// <returns>リソースのポインタ</returns>
	ID3D12Resource* GetResource(int32_t resourceID) const;



	/// <summary>
	/// Readbackの要求
	/// </summary>
	/// <param name="resourceID">リソースID</param>
	/// <param name="bytes">読み出すバイト数</param>
	/// <returns>Readbackトークン</returns>
	int32_t RequestReadback(int32_t resourceID, size_t bytes);

	/// <summary>
	/// Readbackの結果を取得
	/// </summary>
	/// <param name="token">Readbackトークン</param>
	/// <param name="outData">結果を格納するバッファ</param>
	/// <param name="bytes">読み出すバイト数</param>
	/// <returns>結果が利用可能であればtrue</returns>
	bool TryGetReadbackResult(int32_t readbackToken, void* outData, size_t bytes);

	// Engine側でDispatchComputeObjects()の直後に1回呼ぶ。実際のコピー命令をここで積む
	void FlushPendingReadbackRequests();

	void TransitionToUAV(int32_t resourceID, ID3D12GraphicsCommandList6* cmdList);
	void TransitionToSRV(int32_t resourceID, ID3D12GraphicsCommandList6* cmdList);

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
	int32_t nextResourceID_ = 0;
	std::unordered_map<int32_t, BufferKind> kindMap_{};

	int32_t nextReadbackToken_ = 0;
	std::unordered_map<int32_t, PendingReadback> pendingReadbacks_{};

	// TextureLoaderにもmodelLoaderにもある中間リソース　いつか統合。毎フレーム解放
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> pendingIntermediates_{};

};



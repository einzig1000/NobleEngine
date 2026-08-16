#pragma once
#include <externals/DirectXTex/DirectXTex.H>
#include <EngineDefinition/EngineDefinition.h>
#include <d3d12.h>
#include <wrl.h>
#include <DirectX/DirectXManager.h>

namespace Dx12ResourceFactory
{
    /// <summary>
    /// バッファリソースを作成する関数
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="sizeInBytes">バッファのサイズ (バイト単位)</param>
    /// <returns>作成されたバッファリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
        ID3D12Device2* device, size_t sizeInBytes);

    /// <summary>
    /// 定数バッファリソースを作成する関数(256の倍数になる
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="sizeInBytes">バッファのサイズ (バイト単位)</param>
    /// <returns>作成された定数バッファリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateConstantBufferResource(
        ID3D12Device2* device, size_t sizeInBytes);

    /// <summary>
	/// GPUリソースを作成する関数
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="sizeInBytes">バッファのサイズ (バイト単位)</param>
    /// <returns>作成された定数バッファリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBufferResource(
		ID3D12Device2* device, size_t sizeInBytes, D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE);

    /// <summary>
    /// 読み戻し用(Readbackヒープ)のバッファリソースを作成する関数
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="sizeInBytes">バッファのサイズ (バイト単位)</param>
    /// <returns>作成されたバッファリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateReadbackResource(
        ID3D12Device2* device, size_t sizeInBytes);


    template <typename T>
    [[nodiscard]]
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateUploadResource(
        ID3D12Resource* buffer,
        const std::vector<T>& data,
        ID3D12Device2* device,
        ID3D12GraphicsCommandList6* commandList)
    {
        D3D12_SUBRESOURCE_DATA subresourceData{};
        subresourceData.pData = data.data();                         // データの先頭ポインタ
        subresourceData.RowPitch = data.size() * sizeof(T);          // 全体のバイトサイズ
        subresourceData.SlicePitch = subresourceData.RowPitch;       // バッファなのでRowPitchと同じ
        // 中間リソース(Uploadヒープ)に必要なサイズを取得
        uint64_t intermediateSize = GetRequiredIntermediateSize(buffer, 0, 1);
        // 既存のFactoryを利用して中間リソースを作成
        Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource =
            Dx12ResourceFactory::CreateBufferResource(device, intermediateSize);
        // コマンドリストにコピー処理を記録
        UpdateSubresources(commandList, buffer, intermediateResource.Get(), 0, 0, 1, &subresourceData);
        // ResourceStateをCOPY_DESTからGENERIC_READに変更
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = buffer;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
        commandList->ResourceBarrier(1, &barrier);

        return intermediateResource;
    }

	/// <summary>
	/// テクスチャのメタデータを基に DirectX 12 のテクスチャリソースを作成する関数
	/// </summary>
	/// <param name="device">DirectX 12 デバイス</param>
	/// <param name="metadata">テクスチャのメタデータ</param>
	/// <returns>作成されたテクスチャリソース</returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
        ID3D12Device2* device, const DirectX::TexMetadata& metadata);

    /// <summary>
	/// 深度ステンシルバッファリソースを作成する関数
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="width">バッファの幅</param>
    /// <param name="height">バッファの高さ</param>
    /// <returns>作成された深度ステンシルバッファリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilResource(
		ID3D12Device2* device, UINT width, UINT height);

	/// <summary>
	/// レンダーターゲット用のテクスチャリソースを作成する関数000000
	/// </summary>
	/// <param name="device">DirectX 12 デバイス</param>
	/// <param name="width">テクスチャの幅</param>
	/// <param name="height">テクスチャの高さ</param>
	/// <param name="format">テクスチャのフォーマット</param>
	/// <returns>作成されたレンダーターゲット用のテクスチャリソース</returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTargetResource(
		ID3D12Device2* device, UINT width, UINT height, DXGI_FORMAT format, float clearColor[4]);
}

namespace Dx12ResourceTransition
{
	/// <summary>
	/// リソースの状態を遷移させる関数
	/// </summary>
	/// <param name="commandList">DirectX 12 コマンドリスト</param>
	/// <param name="resource">遷移させるリソース</param>
	/// <param name="beforeState">遷移前のリソース状態</param>
	/// <param name="afterState">遷移後のリソース状態</param>
	void Transition(
		ID3D12GraphicsCommandList6* commandList,
		ID3D12Resource* resource,
		D3D12_RESOURCE_STATES beforeState,
		D3D12_RESOURCE_STATES afterState);
}
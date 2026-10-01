#pragma once
#include <externals/DirectXTex/DirectXTex.H>
#include <EngineDefinition/EngineDefinition.h>
#include <d3d12.h>
#include <wrl.h>
#include <DirectX/DirectXManager.h>
#include <ranges>
#include <type_traits>

/// <summary>
/// GPUへそのままコピーできる範囲(vector / std::array / 生配列 / span など)
/// 「メモリが連続している」「要素数が分かる」「memcpyしてよい型」の3つを満たすもの
/// </summary>
template<typename R>
concept GpuUploadRange =
std::ranges::contiguous_range<R> &&
std::ranges::sized_range<R> &&
std::is_trivially_copyable_v<std::ranges::range_value_t<R>>;

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


    /// <summary>
    /// デフォルトヒープのバッファにデータを送る(中間リソースを作ってコピー命令を積む)
    /// </summary>
    /// <param name="buffer">転送先バッファ(サイズがsizeInBytesと同じであること)</param>
    /// <param name="data">送るデータの先頭</param>
    /// <param name="sizeInBytes">送るバイト数</param>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="commandList">コピー命令を積むコマンドリスト</param>
    /// <returns>中間リソース(GPUのコピーが終わるまで保持すること)</returns>
    [[nodiscard]]
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateUploadResource(
        ID3D12Resource* buffer,
        const void* data,
        size_t sizeInBytes,
        ID3D12Device2* device,
        ID3D12GraphicsCommandList6* commandList);

    /// <summary>
    /// コンテナ版。(先頭ポインタ, バイト数)に変えて上の関数に渡すだけ
    /// </summary>
    template<GpuUploadRange Range>
    [[nodiscard]]
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateUploadResource(
        ID3D12Resource* buffer,
        const Range& data,
        ID3D12Device2* device,
        ID3D12GraphicsCommandList6* commandList)
    {
        using T = std::ranges::range_value_t<Range>;
        return CreateUploadResource(buffer, std::ranges::data(data), std::ranges::size(data) * sizeof(T), device, commandList);
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
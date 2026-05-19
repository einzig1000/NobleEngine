#pragma once
#include <externals/DirectXTex/DirectXTex.H>
#include <d3d12.h>
#include <wrl.h>

namespace Dx12ResourceFactory
{
    /// <summary>
    /// バッファリソースを作成する関数
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="sizeInBytes">バッファのサイズ (バイト単位)</param>
    /// <returns>作成されたバッファリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
        ID3D12Device* device, size_t sizeInBytes);

    /// <summary>
    /// 定数バッファリソースを作成する関数(256の倍数になる
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="sizeInBytes">バッファのサイズ (バイト単位)</param>
    /// <returns>作成された定数バッファリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateConstantBufferResource(
        ID3D12Device* device, size_t sizeInBytes);

	/// <summary>
	/// テクスチャのメタデータを基に DirectX 12 のテクスチャリソースを作成する関数
	/// </summary>
	/// <param name="device">DirectX 12 デバイス</param>
	/// <param name="metadata">テクスチャのメタデータ</param>
	/// <returns>作成されたテクスチャリソース</returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
        ID3D12Device* device, const DirectX::TexMetadata& metadata);

    /// <summary>
	/// 深度ステンシルバッファリソースを作成する関数
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="width">バッファの幅</param>
    /// <param name="height">バッファの高さ</param>
    /// <returns>作成された深度ステンシルバッファリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilResource(
		ID3D12Device* device, UINT width, UINT height);

	/// <summary>
	/// レンダーターゲット用のテクスチャリソースを作成する関数
	/// </summary>
	/// <param name="device">DirectX 12 デバイス</param>
	/// <param name="width">テクスチャの幅</param>
	/// <param name="height">テクスチャの高さ</param>
	/// <param name="format">テクスチャのフォーマット</param>
	/// <returns>作成されたレンダーターゲット用のテクスチャリソース</returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTargetResource(
		ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format);
}
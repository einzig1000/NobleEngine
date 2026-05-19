#include "Dx12ResourceFactory.h"
#include <Utilities/Logger/Logger.h>

namespace Dx12ResourceFactory
{
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
        ID3D12Device* device, size_t sizeInBytes)
    {
        // リソース記述子を作成
        D3D12_RESOURCE_DESC resourceDesc{};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Width = sizeInBytes;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        // 利用するHeapの設定
        D3D12_HEAP_PROPERTIES heapProperties{};
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

        // リソースを作成
        Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
        HRESULT hr = device->CreateCommittedResource(
            &heapProperties,        // ヒープのプロパティ
            D3D12_HEAP_FLAG_NONE,   // ヒープフラグ
            &resourceDesc,          // リソースの記述子
            D3D12_RESOURCE_STATE_GENERIC_READ,           // 初期状態
            nullptr,                // Clear値 (バッファの場合はnullptr)
            IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()) // ID3D12Resourceポインタを取得
        );

		// 失敗した場合はエラーをログに出力してnullptrを返す
        if (FAILED(hr) || !resource)
        {
            const HRESULT removed = device->GetDeviceRemovedReason();
            Log("CreateBufferResource()に失敗しました。hr=%s removedReason=%s",
                HrToString(hr),
                HrToString(removed));
            return nullptr;
        }

        resource->SetName(L"CreateBufferResource()");

        return resource; // 作成したリソースを返す
    };

    Microsoft::WRL::ComPtr<ID3D12Resource> CreateConstantBufferResource(ID3D12Device* device, size_t sizeInBytes)
    {
        size_t ConstantSize;
        ConstantSize = (sizeInBytes + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);

        return CreateBufferResource(device, ConstantSize);
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
    {
        // リソース記述子を作成
        D3D12_RESOURCE_DESC resourceDesc{};
        resourceDesc.Width = UINT(metadata.width);
        resourceDesc.Height = UINT(metadata.height);
        resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
        resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or 配列Textureの配列数
        resourceDesc.Format = metadata.format; // TextureのFormat
        resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。１固定
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使ってるのは２次元

        // 利用するHeapの設定
        D3D12_HEAP_PROPERTIES heapProperties{};
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

        // リソースを作成
        Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
        HRESULT hr = device->CreateCommittedResource(
            &heapProperties,        // ヒープのプロパティ
            D3D12_HEAP_FLAG_NONE,   // ヒープフラグ
            &resourceDesc,          // リソースの記述子
            D3D12_RESOURCE_STATE_COMMON, // 初期状態
            nullptr,                // Clear値 (テクスチャの場合はnullptr)
            IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()) // ID3D12Resourceポインタを取得
        );

        // 失敗した場合はエラーをログに出力してnullptrを返す
        if (FAILED(hr) || !resource)
        {
            const HRESULT removed = device->GetDeviceRemovedReason();
			Log("CreateTextureResource()に失敗しました。hr=%s removedReason=%s",
                HrToString(hr),
                HrToString(removed));
            return nullptr;
        }

        resource->SetName(L"CreateTextureResource()");

        return resource; // 作成したリソースを返す
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilResource(ID3D12Device* device, UINT width, UINT height)
    {
        // リソース記述子を作成
        D3D12_RESOURCE_DESC depthStencilDesc = {};
        depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthStencilDesc.Width = width;
        depthStencilDesc.Height = height;
        depthStencilDesc.DepthOrArraySize = 1;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        // 利用するHeapの設定
        D3D12_HEAP_PROPERTIES heapProperties{};
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

        // クリア値の設定
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

        // リソースを作成
        Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
        HRESULT hr = device->CreateCommittedResource(
            &heapProperties,        // ヒープのプロパティ
            D3D12_HEAP_FLAG_NONE,   // ヒープフラグ
            &depthStencilDesc,      // リソースの記述子
            D3D12_RESOURCE_STATE_COMMON, // 初期状態
            &clearValue,            // Clear値
			IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()) // ID3D12Resourceポインタを取得
        );

		if (FAILED(hr) || !resource)
		{
			const HRESULT removed = device->GetDeviceRemovedReason();
			Log("CreateDepthStencilResource()に失敗しました。 hr=%s removedReason=%s",
				HrToString(hr),
				HrToString(removed));
			return nullptr;
		}

        resource->SetName(L"CreateDepthStencilResource()");

		return resource;
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTargetResource(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format)
    {
        // リソース記述子を作成
        D3D12_RESOURCE_DESC resourceDesc{};
        resourceDesc.Width = width;
        resourceDesc.Height = height;
        resourceDesc.MipLevels = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.Format = format;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // レンダーターゲット用のテクスチャは2D固定

        // 利用するHeapの設定
        D3D12_HEAP_PROPERTIES heapProperties{};
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

		// クリア値の設定
		D3D12_CLEAR_VALUE clearValue{};
		clearValue.Format = format;
		clearValue.Color[0] = 0.396078f;
        clearValue.Color[1] = 0.894117f;
        clearValue.Color[2] = 1.0f;
		clearValue.Color[3] = 1.0f;

        // リソースを作成
        Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
        HRESULT hr = device->CreateCommittedResource(
            &heapProperties,        // ヒープのプロパティ
            D3D12_HEAP_FLAG_NONE,   // ヒープフラグ
            &resourceDesc,          // リソースの記述子
            D3D12_RESOURCE_STATE_COMMON, // 初期状態
            &clearValue,            // Clear値
            IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()) // ID3D12Resourceポインタを取得
        );

        // 失敗した場合はエラーをログに出力してnullptrを返す
        if (FAILED(hr) || !resource)
        {
            const HRESULT removed = device->GetDeviceRemovedReason();
            Log("CreateRenderTargetResource()に失敗しました。 hr=%s removedReason=%s",
                HrToString(hr),
                HrToString(removed));
            return nullptr;
        }

        resource->SetName(L"CreateRenderTargetResource()");

        return resource; // 作成したリソースを返す
    }
}
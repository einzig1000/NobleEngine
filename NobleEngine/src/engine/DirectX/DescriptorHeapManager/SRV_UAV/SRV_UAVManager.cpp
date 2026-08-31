#include "DirectX/DescriptorHeapManager/SRV_UAV/SRV_UAVManager.h"
#include "Utilities/functions.h"
#include "Utilities/Logger/Logger.h"

SRV_UAVManager::SRV_UAVManager(ID3D12Device2* device)
    :device_(device)
{
    Log("コンストラクタ実行開始 : SRV_UAVManager");

    // SRVスロット一つ分のサイズ取得
    descriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // SRV用ディスクリプタヒープ作成
	textureCapacity_ = 256;
	textureCubeCapacity_ = 16;
	bufferCapacity_ = 1024 * 16 * 4;
    capacity_ = textureCapacity_ + textureCubeCapacity_ + bufferCapacity_;
    nextTextureIndex_ = 0;
	nextTextureCubeIndex_ = textureCapacity_;
	nextBufferIndex_ = textureCapacity_ + textureCubeCapacity_;
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
    DescriptorHeapDesc.NumDescriptors = capacity_;
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    HRESULT hr = device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap_));
    assert(SUCCEEDED(hr));

    Log("成功");
}

SRV_UAVManager::~SRV_UAVManager()
{
    Log("デストラクタ実行成功 : SRV_UAVManager");
}
  
uint32_t SRV_UAVManager::Allocate(ResourceType type)
{
    switch (type)
    {
    case SRV_UAVManager::ResourceType::Texture2D:
		if (nextTextureIndex_ >= textureCapacity_)
        {
            Log("SRVのテクスチャスロットが足りません。容量を増やしてください。");
            assert(false);
            return UINT32_MAX;
        }
		return nextTextureIndex_++;
        break;
    case SRV_UAVManager::ResourceType::TextureCube:
        if (nextTextureCubeIndex_ >= textureCapacity_ + textureCubeCapacity_)
        {
            Log("SRVのテクスチャキューブスロットが足りません。容量を増やしてください。");
            assert(false);
            return UINT32_MAX;
		}
		return nextTextureCubeIndex_++;
        break;
    case SRV_UAVManager::ResourceType::StructuredBuffer:
        if (nextBufferIndex_ >= capacity_)
        {
            Log("SRVのバッファスロットが足りません。容量を増やしてください。");
            assert(false);
            return UINT32_MAX;
		}
		return nextBufferIndex_++;
        break;
    default:
        assert(false);
		return UINT32_MAX;
        break;
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE SRV_UAVManager::GetCPUHandleAt(uint32_t i) const
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += (descriptorSize_ * i);
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE SRV_UAVManager::GetGPUHandleAt(uint32_t i) const
{
    D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += (descriptorSize_ * i);
    return handle;
}

SRV_UAVManager::Allocation SRV_UAVManager::CreateUAVforStructuredBuffer(ID3D12Resource* resource, UINT numElements, UINT structureByteStride)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = numElements;
    uavDesc.Buffer.StructureByteStride = structureByteStride;
    uavDesc.Buffer.CounterOffsetInBytes = 0;
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
    return CreateUAV(resource, &uavDesc, ResourceType::StructuredBuffer);
}

SRV_UAVManager::Allocation SRV_UAVManager::CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc, ResourceType type)
{
    // 次スロットのインデックス取得
    uint32_t index = Allocate(type);

    // ハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE cpu = GetCPUHandleAt(index);
    D3D12_GPU_DESCRIPTOR_HANDLE gpu = GetGPUHandleAt(index);

    // SRV作成
    device_->CreateShaderResourceView(resource, desc, cpu);

    Allocation alloc{ index, cpu, gpu, ViewType::SRV };
    return alloc;
}
SRV_UAVManager::Allocation SRV_UAVManager::CreateUAV(ID3D12Resource* resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc, ResourceType type)
{
    // 次スロットのインデックス取得
    uint32_t index = Allocate(type);

    D3D12_CPU_DESCRIPTOR_HANDLE cpu = GetCPUHandleAt(index);
    D3D12_GPU_DESCRIPTOR_HANDLE gpu = GetGPUHandleAt(index);

    // UAV作成
    device_->CreateUnorderedAccessView(resource, nullptr, desc, cpu);

    Allocation alloc{ index, cpu, gpu, ViewType::UAV };
    return alloc;
}

SRV_UAVManager::Allocation SRV_UAVManager::CreateSRVforTexture(ID3D12Resource* resource, const DirectX::TexMetadata& metadata)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
    return CreateSRV(resource, &srvDesc, ResourceType::Texture2D);
}
SRV_UAVManager::Allocation SRV_UAVManager::CreateSRVforDDS(ID3D12Resource* resource, const DirectX::TexMetadata& metadata)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
    srvDesc.TextureCube.MipLevels = UINT_MAX;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
    return CreateSRV(resource, &srvDesc, ResourceType::TextureCube);
}
SRV_UAVManager::Allocation SRV_UAVManager::CreateSRVforRenderTarget(ID3D12Resource* resource)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    D3D12_RESOURCE_DESC resDesc = resource->GetDesc();
    srvDesc.Format = resDesc.Format; // リソースと一致
    //srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    return CreateSRV(resource, &srvDesc, ResourceType::Texture2D);
}
SRV_UAVManager::Allocation SRV_UAVManager::CreateSRVforDepthTexture(ID3D12Resource* resource)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    return CreateSRV(resource, &srvDesc, ResourceType::Texture2D);
}
SRV_UAVManager::Allocation SRV_UAVManager::CreateSRVforStructuredBuffer(ID3D12Resource* resource, UINT numElements, UINT structureByteStride)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    srvDesc.Buffer.NumElements = numElements;
    srvDesc.Buffer.StructureByteStride = structureByteStride;
	return CreateSRV(resource, &srvDesc, ResourceType::StructuredBuffer);
}


void SRV_UAVManager::RewriteSRVforStructuredBuffer(Allocation& allocation, ID3D12Resource* resource, UINT numElements, UINT structureByteStride)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    srvDesc.Buffer.NumElements = numElements;
    srvDesc.Buffer.StructureByteStride = structureByteStride;
    device_->CreateShaderResourceView(resource, &srvDesc, allocation.cpu);
}
#include "DirectX/DescriptorHeapManager/SRV_UAV/SRV_UAVManager.h"
#include <Utilities/functions.h>
#include <Utilities/Logger/Logger.h>

SRV_UAVManager::SRV_UAVManager(ID3D12Device2* device)
    : device_(device)
    , heap_(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024 * 16 * 8, true, "SRV/UAV")
{
    Logger::Info("コンストラクタ実行成功 : SRV_UAVManager");
}

SRV_UAVManager::~SRV_UAVManager()
{
    Logger::Info("デストラクタ実行成功 : SRV_UAVManager");
}

SRV_UAVManager::Allocation SRV_UAVManager::CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
{
    // 次スロットのインデックス取得
    uint32_t index = heap_.Allocate();

    // ハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE cpu = heap_.GetCPUHandleAt(index);
    D3D12_GPU_DESCRIPTOR_HANDLE gpu = heap_.GetGPUHandleAt(index);

    // SRV作成
    device_->CreateShaderResourceView(resource, desc, cpu);

    Allocation alloc{ index, cpu, gpu, ViewType::SRV };
    return alloc;
}
SRV_UAVManager::Allocation SRV_UAVManager::CreateUAV(ID3D12Resource* resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc)
{
    // 次スロットのインデックス取得
    uint32_t index = heap_.Allocate();

    D3D12_CPU_DESCRIPTOR_HANDLE cpu = heap_.GetCPUHandleAt(index);
    D3D12_GPU_DESCRIPTOR_HANDLE gpu = heap_.GetGPUHandleAt(index);

    // UAV作成
    device_->CreateUnorderedAccessView(resource, nullptr, desc, cpu);

    Allocation alloc{ index, cpu, gpu, ViewType::UAV };
    return alloc;
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
    return CreateUAV(resource, &uavDesc);
}

SRV_UAVManager::Allocation SRV_UAVManager::CreateSRVforTexture(ID3D12Resource* resource, const DirectX::TexMetadata& metadata)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
    return CreateSRV(resource, &srvDesc);
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
    return CreateSRV(resource, &srvDesc);
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
    return CreateSRV(resource, &srvDesc);
}
SRV_UAVManager::Allocation SRV_UAVManager::CreateSRVforDepthTexture(ID3D12Resource* resource)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    return CreateSRV(resource, &srvDesc);
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
	return CreateSRV(resource, &srvDesc);
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

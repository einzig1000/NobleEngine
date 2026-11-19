#pragma once
#include <d3d12.h>
#include <cstdint>
#include <externals/DirectXTex/DirectXTex.h>
#include <DirectX/DescriptorHeapManager/DescriptorHeap.h>


class SRV_UAVManager
{
public:

    enum class ViewType
    {
        SRV,
        UAV
    };

    struct Allocation
    {
        uint32_t index = UINT32_MAX;
        D3D12_CPU_DESCRIPTOR_HANDLE cpu{};
        D3D12_GPU_DESCRIPTOR_HANDLE gpu{};
        ViewType type = ViewType::SRV;
    };

    enum class ResourceType
    {
        Texture2D,
        TextureCube,
        StructuredBuffer
    };


    SRV_UAVManager(ID3D12Device2* device);
    ~SRV_UAVManager();

    ID3D12DescriptorHeap* GetSRVDescriptorHeap() const { return heap_.GetD3D12Heap(); }

    // スロット管理はヒープに任せる
    uint32_t Allocate() { return heap_.Allocate(); }
    void Free(uint32_t index) { heap_.Free(index); }
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleAt(uint32_t index) const { return heap_.GetCPUHandleAt(index); }
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleAt(uint32_t index) const { return heap_.GetGPUHandleAt(index); }

    // 使用数などの表示用
    const DescriptorHeap& GetHeap() const { return heap_; }

    Allocation CreateUAVforStructuredBuffer(ID3D12Resource* resource, UINT numElements, UINT structureByteStride);

    Allocation CreateSRVforTexture(ID3D12Resource* resource, const DirectX::TexMetadata& metadata);
    Allocation CreateSRVforDDS(ID3D12Resource* resource, const DirectX::TexMetadata& metadata);
    Allocation CreateSRVforStructuredBuffer(ID3D12Resource* resource, UINT numElements, UINT structureByteStride);
    Allocation CreateSRVforRenderTarget(ID3D12Resource* resource);
    Allocation CreateSRVforDepthTexture(ID3D12Resource* resource);
    void RewriteSRVforStructuredBuffer(Allocation& allocation, ID3D12Resource* resource, UINT numElements, UINT structureByteStride);

private:

    // SRV・UAVの作成とスロットの割り当て
    Allocation CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc);
    Allocation CreateUAV(ID3D12Resource* resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc);

    // デバイス
    ID3D12Device2* device_ = nullptr;
    // SRV・UAV用のディスクリプタヒープ
    DescriptorHeap heap_;
};

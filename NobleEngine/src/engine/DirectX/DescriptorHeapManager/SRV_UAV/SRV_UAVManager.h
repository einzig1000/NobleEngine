#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <externals/DirectXTex/DirectXTex.h>


class SRV_UAVManager
{
public:

    enum class ViewType
    {
        SRV,
        UAV,
        CBV
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
    

    SRV_UAVManager(ID3D12Device* device);
    ~SRV_UAVManager();

    ID3D12DescriptorHeap* GetSRVDescriptorHeap() const { return descriptorHeap_.Get(); }
    uint32_t GetDescriptorSizeSRV() const { return descriptorSize_; }

    // 空いてるスロットインデックスを取得しnextIndex_をインクリメント
    uint32_t Allocate(ResourceType type = ResourceType::Texture2D);

    // Allocate()で取得したスロットインデックスのCPU/GPUハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleAt(uint32_t index) const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleAt(uint32_t index) const;

    Allocation CreateSRVforTexture(ID3D12Resource* resource, const DirectX::TexMetadata& metadata);
    Allocation CreateSRVforDDS(ID3D12Resource* resource, const DirectX::TexMetadata& metadata);
    Allocation CreateSRVforStructuredBuffer(ID3D12Resource* resource, UINT numElements, UINT structureByteStride);
	Allocation CreateSRVforRenderTarget(ID3D12Resource* resource);
    void RewriteSRVforStructuredBuffer(Allocation& allocation, ID3D12Resource* resource, UINT numElements, UINT structureByteStride);

private:
    // SRVの作成とスロットの割り当て
    Allocation CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc, ResourceType type);

    // デバイス
    ID3D12Device* device_ = nullptr;

    // SRV用のディスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

    // ディスクリプタサイズ
    uint32_t descriptorSize_;
    // 最大スロット数
    uint32_t capacity_ = 0;
    uint32_t textureCapacity_ = 0;
    uint32_t textureCubeCapacity_ = 0;
    uint32_t bufferCapacity_ = 0;
    // 次のスロットインデックス
    uint32_t nextTextureIndex_ = 0;
	uint32_t nextTextureCubeIndex_ = 0;
    uint32_t nextBufferIndex_ = 0;
};

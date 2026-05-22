#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <Windows.h>
#include <dxgiformat.h>
#include <unordered_map>
#include <memory>
#include <string>
#include <DirectX/DescriptorHeapManager/DescriptorHeapManager.h>
#include <DirectX/RenderTextureManager/RenderTextureID.h>

struct RenderTexture
{
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	Microsoft::WRL::ComPtr<ID3D12Resource> dsvResource;
	RTVManager::Allocation rtvAlloc{};
	DSVManager::Allocation dsvAlloc{};
	SRV_UAVManager::Allocation srvAlloc{};
    UINT width = 0;
    UINT height = 0;
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};
	D3D12_RESOURCE_STATES currentState = D3D12_RESOURCE_STATE_COMMON;
    D3D12_RESOURCE_STATES dsvCurrentState = D3D12_RESOURCE_STATE_COMMON;
};

class DescriptorHeapManager;

class RenderTextureManager
{
public:
	RenderTextureManager(ID3D12Device* device, DescriptorHeapManager* descriptorHeapManager);
	~RenderTextureManager();

    // 作成
    RenderTexture* CreateRenderTarget(UINT width, UINT height, DXGI_FORMAT format, RenderTextureID renderTextureID);
    RenderTexture* CreateDepthTexture(UINT width, UINT height, DXGI_FORMAT format);

    // 取得
    RenderTexture* Get(RenderTextureID renderTextureID) const;

    // リサイズ
    void ResizeAllWindowDependent(UINT newWidth, UINT newHeight);

    // 破棄
    void Destroy(RenderTextureID renderTextureID);

private:
    ID3D12Device* device_;
    DescriptorHeapManager* descriptorHeapManager_;
    std::unordered_map<RenderTextureID, std::unique_ptr<RenderTexture>> textures_;
};


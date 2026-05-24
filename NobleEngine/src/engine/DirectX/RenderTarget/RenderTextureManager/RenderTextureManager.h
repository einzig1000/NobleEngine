#pragma once
#include <memory>
#include <string>
#include <d3d12.h>
#include <Windows.h>
#include <dxgiformat.h>
#include <unordered_map>
#include <DirectX/RenderTarget/RenderTargetStruct.h>

class DescriptorHeapManager;

class RenderTextureManager
{
public:
	RenderTextureManager(ID3D12Device* device, DescriptorHeapManager* descriptorHeapManager);
	~RenderTextureManager();

    // テクスチャ作成
    int32_t CreateRenderTarget(UINT width, UINT height, DXGI_FORMAT format, std::string textureName);
    RenderTarget* CreateDepthTexture(UINT width, UINT height, DXGI_FORMAT format);

    // データ取得
    RenderTarget* Get(int32_t textureID) const;
    RenderTarget* Get(const std::string& textureName) const;

    // リサイズ
    void ResizeAllWindowDependent(UINT newWidth, UINT newHeight);

private:
    ID3D12Device* device_;
    DescriptorHeapManager* descriptorHeapManager_;
	std::unordered_map<std::string, int32_t> nameToIDMap_;
    std::vector<std::unique_ptr<RenderTarget>> textures_;
};


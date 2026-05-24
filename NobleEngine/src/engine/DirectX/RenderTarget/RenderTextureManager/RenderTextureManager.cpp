#include "RenderTextureManager.h"
#include <Utilities/Logger/Logger.h>
#include <DirectX/Resource/Dx12ResourceFactory.h>
#include <DirectX/DescriptorHeapManager/DescriptorHeapManager.h>

RenderTextureManager::RenderTextureManager(ID3D12Device* device, DescriptorHeapManager* descriptorHeapManager)
    : device_(device), descriptorHeapManager_(descriptorHeapManager)
{
}

RenderTextureManager::~RenderTextureManager()
{}

int32_t RenderTextureManager::CreateRenderTarget(UINT width, UINT height, DXGI_FORMAT format, std::string textureName)
{
	// 既に作成されていたらそのIDを返す
	auto it = nameToIDMap_.find(textureName);
	if (it != nameToIDMap_.end())
	{
		return it->second;
	}

    auto rt = std::make_unique<RenderTarget>();
    rt->width = width;
    rt->height = height;
    rt->format = format;
    
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rt->resource = Dx12ResourceFactory::CreateRenderTargetResource(device_, width, height, format);
    rt->rtvAlloc = descriptorHeapManager_->GetRTVManager()->CreateRTV(rt->resource.Get(), &rtvDesc);
    rt->srvAlloc = descriptorHeapManager_->GetSRV_UAVManager()->CreateSRVforRenderTarget(rt->resource.Get());

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	rt->dsvResource = Dx12ResourceFactory::CreateDepthStencilResource(device_, width, height);
	rt->dsvAlloc = descriptorHeapManager_->GetDSVManager()->CreateDSV(rt->dsvResource.Get(), &dsvDesc);

    // 5) Viewport / Scissor 初期化
    rt->viewport.TopLeftX = 0.0f;
    rt->viewport.TopLeftY = 0.0f;
    rt->viewport.Width = static_cast<float>(width);
    rt->viewport.Height = static_cast<float>(height);
    rt->viewport.MinDepth = 0.0f;
    rt->viewport.MaxDepth = 1.0f;

    rt->scissorRect.left = 0;
    rt->scissorRect.top = 0;
    rt->scissorRect.right = static_cast<LONG>(width);
    rt->scissorRect.bottom = static_cast<LONG>(height);

	rt->state = D3D12_RESOURCE_STATE_COMMON;
    rt->dsvState = D3D12_RESOURCE_STATE_COMMON;

	nameToIDMap_[textureName] = rt->srvAlloc.index;
	if (textures_.size() <= static_cast<size_t>(rt->srvAlloc.index))
	{
		textures_.resize(rt->srvAlloc.index + 1);
	}
	textures_[rt->srvAlloc.index] = std::move(rt);

	return nameToIDMap_[textureName];
}

RenderTarget* RenderTextureManager::CreateDepthTexture(UINT width, UINT height, DXGI_FORMAT format)
{
	return nullptr;
}

RenderTarget* RenderTextureManager::Get(int32_t textureID) const
{
	if (textureID < 0 || textureID >= static_cast<int32_t>(textures_.size()))
	{
		// IDが範囲外の場合はエラー
		Log("テクスチャIDが範囲外です:%d", textureID);
		assert(false);
		return nullptr;
	}
    return textures_[textureID].get();
}

RenderTarget* RenderTextureManager::Get(const std::string& textureName) const
{
	auto it = nameToIDMap_.find(textureName);
	if (it != nameToIDMap_.end())
	{
		return textures_[it->second].get();
	}
	return nullptr;
}

void RenderTextureManager::ResizeAllWindowDependent(UINT newWidth, UINT newHeight)
{}

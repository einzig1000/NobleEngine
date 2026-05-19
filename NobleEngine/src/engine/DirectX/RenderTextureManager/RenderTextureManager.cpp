#include "RenderTextureManager.h"
#include <DirectX/Resource/Dx12ResourceFactory.h>
#include <Window/WindowManager.h>

RenderTextureManager::RenderTextureManager(ID3D12Device* device, DescriptorHeapManager* descriptorHeapManager)
    : device_(device), descriptorHeapManager_(descriptorHeapManager)
{
	CreateRenderTarget(UINT(WindowManager::winWidth_), UINT(WindowManager::winHeight_), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
}

RenderTextureManager::~RenderTextureManager()
{}

RenderTexture* RenderTextureManager::CreateRenderTarget(UINT width, UINT height, DXGI_FORMAT format)
{
    auto rt = std::make_unique<RenderTexture>();
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

	std::string name = "RenderTarget_" + std::to_string(textures_.size());
    textures_.emplace(name, std::move(rt));
	return textures_.at(name).get();
}


RenderTexture* RenderTextureManager::CreateDepthTexture(UINT width, UINT height, DXGI_FORMAT format)
{
	return nullptr;
}

RenderTexture* RenderTextureManager::Get(const std::string& name) const
{
	if (textures_.find(name) != textures_.end())
	{
		return textures_.at(name).get();
	}
	else
	{
		return nullptr;
	}
}

void RenderTextureManager::ResizeAllWindowDependent(UINT newWidth, UINT newHeight)
{}

void RenderTextureManager::Destroy(const std::string & name)
{}

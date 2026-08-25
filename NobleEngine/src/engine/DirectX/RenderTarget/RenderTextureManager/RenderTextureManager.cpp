#include "RenderTextureManager.h"
#include <Utilities/Logger/Logger.h>
#include <DirectX/ResourceUtilities/ResourceUtilities.h>
#include <Utilities/Converter/StringConverter/StringConverter.h>
#include <filesystem>

RenderTextureManager::RenderTextureManager(ID3D12Device2* device, ID3D12CommandQueue* commandQueue, DescriptorHeapManager* descriptorHeapManager)
    : device_(device), commandQueue_(commandQueue), descriptorHeapManager_(descriptorHeapManager)
{}

RenderTextureManager::~RenderTextureManager()
{}

int32_t RenderTextureManager::CreateRenderTarget(UINT width, UINT height, DXGI_FORMAT format, std::string label, Vector4 clearColor)
{
	// 既に作成されていたらそのIDを返す
	RenderTarget* renderTarget = Get(label);
	if (renderTarget != nullptr)
	{
        return renderTarget->colorsrvAlloc.index;
	}

    Log("レンダーテクスチャ作成開始:%s", label.c_str());
    
    auto rt = std::make_unique<RenderTarget>();
	rt->name = label;
    rt->width = width;
    rt->height = height;
    rt->format = format;
    rt->clearColor[0] = clearColor.x;
    rt->clearColor[1] = clearColor.y;
    rt->clearColor[2] = clearColor.z;
    rt->clearColor[3] = clearColor.w;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rt->colorResource = Dx12ResourceFactory::CreateRenderTargetResource(device_, width, height, format, rt->clearColor);
    rt->rtvAlloc = descriptorHeapManager_->GetRTVManager()->CreateRTV(rt->colorResource.Get(), &rtvDesc);
    rt->colorsrvAlloc = descriptorHeapManager_->GetSRV_UAVManager()->CreateSRVforRenderTarget(rt->colorResource.Get());

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	rt->depthResource = Dx12ResourceFactory::CreateDepthStencilResource(device_, width, height);
	rt->dsvAlloc = descriptorHeapManager_->GetDSVManager()->CreateDSV(rt->depthResource.Get(), &dsvDesc);
    rt->depthsrvAlloc = descriptorHeapManager_->GetSRV_UAVManager()->CreateSRVforDepthTexture(rt->depthResource.Get());

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

	int32_t id = static_cast<int32_t>(renderTargets_.size());
	nameToIndexMap_[label] = id;
    idToIndexMap_[rt->colorsrvAlloc.index] = id;
	idToIndexMap_[rt->depthsrvAlloc.index] = id;
	renderTargets_.push_back(std::move(rt));

    Log("成功");

	return renderTargets_.back()->colorsrvAlloc.index;
}

bool RenderTextureManager::SaveTexture(const std::string& filePath, std::string textureName, bool color)
{
	std::string fullPath = color ? filePath + "/" + textureName + ".png" : filePath + "/" + textureName + "_depth.png";
	// もし同じ名前のファイルが存在していたらtextureName2みたいにして保存する
	if (std::filesystem::exists(fullPath))
	{
		int32_t suffix = 2;
		while (true)
		{
			fullPath = filePath + "/" + textureName + std::to_string(suffix) + ".png";
			if (!std::filesystem::exists(fullPath))
			{
				break;
			}
			suffix++;
		}
	}

	RenderTarget* targetRT = Get(textureName);
	if (targetRT == nullptr)
	{
		return false;
	}

    DirectX::ScratchImage resultImage;
    HRESULT hr = CaptureTexture(
        commandQueue_,
        color ? targetRT->colorResource.Get() : targetRT->depthResource.Get(),
        false,
        resultImage,
        color ? targetRT->state : targetRT->dsvState,
        color ? targetRT->state : targetRT->dsvState
    );

    if (FAILED(hr))
    {
        return false;
    }

    const DirectX::Image* img = resultImage.GetImage(0, 0, 0);
    if (!img)
    {
        return false;
    }

    std::wstring wFilePath = StringConverter::Convert(fullPath);

    hr = DirectX::SaveToWICFile(
        *img,
        DirectX::WIC_FLAGS_NONE,
        DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG),
        wFilePath.c_str()
    );

    if (FAILED(hr))
    {
        wprintf(L"SaveToWICFile failed: 0x%08X\n", hr);
        return false;
    }

	return true;
}

bool RenderTextureManager::SaveAllRenderTextures(const std::string& directoryPath, bool color)
{
	for (const auto& pair : nameToIndexMap_)
	{
		const std::string& textureName = pair.first;
		if (!SaveTexture(directoryPath, textureName, color))
		{
			return false;
		}
	}
	return true;
}

RenderTarget* RenderTextureManager::Get(int32_t textureID) const
{
    auto it = idToIndexMap_.find(textureID);
    if (it != idToIndexMap_.end())
    {
        return renderTargets_[it->second].get();
    }
    return nullptr;
}

RenderTarget* RenderTextureManager::Get(const std::string& textureName) const
{
	auto it = nameToIndexMap_.find(textureName);
	if (it != nameToIndexMap_.end())
	{
		return renderTargets_[it->second].get();
	}
	return nullptr;
}

void RenderTextureManager::ResizeAllWindowDependent(UINT newWidth, UINT newHeight)
{}

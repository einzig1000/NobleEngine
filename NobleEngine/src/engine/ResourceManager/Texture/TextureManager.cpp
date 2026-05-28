#include "TextureManager.h"
#include <externals/DirectXTex/d3dx12.h>
#include <externals/DirectXTex/DirectXTex.h>
#include <Utilities/Logger/Logger.h>
#include <DirectX/Resource/Dx12ResourceFactory.h>
#include <DirectX/DirectXManager.h>
#include <definition/definition.h>
#include <filesystem>
#include <cassert>
#include <cctype>
#include <algorithm>

TextureManager::TextureManager(DirectXManager* dxManager)
    : dxManager_(dxManager)
{}

TextureManager::~TextureManager()
{
    intermediateUploadResources_.clear();
}

int32_t TextureManager::LoadTexture(const std::string& filePath)
{
	HRESULT hr = S_OK;

	// ファイルパス型に変換
	std::filesystem::path path(filePath);

    // すでに読み込まれていたらそのテクスチャIDを返す
    auto it = pathToIDMap_.find(filePath);
    if (it != pathToIDMap_.end())
    {
		return it->second;
    }

	Log("テクスチャ読み込み開始:%s", filePath.c_str());

    // 識別子を判定
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	bool dds = (ext == ".dds");

    auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
    auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);
	auto* device = dxManager_->GetDevice();
    auto* srvManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();

    // ボックスを作成
    TextureData text;

	// 画像データとメタデータの作成
    DirectX::ScratchImage image{};
    if (dds)
    {
        hr = DirectX::LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
    else
    {
        hr = DirectX::LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    }
	if (FAILED(hr))
	{
        Log("ファイルが見つかりませんでした:%s", filePath.c_str());
		assert(false);
		return -1;
	}

    // ミップマップの作成
    DirectX::ScratchImage mipImageLocal;
	if (DirectX::IsCompressed(image.GetMetadata().format))
    {
		mipImageLocal = std::move(image);
    }
    else
    {
        hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImageLocal);
		if (FAILED(hr))
		{
			Log("ミップマップの生成に失敗しました:%s HRESULT: 0x%X", filePath.c_str(), hr);
			assert(false);
			return -1;
		}
    }

    // メタデータ・ミップマップを保存
    text.metadata = mipImageLocal.GetMetadata();
    text.mipImage = std::move(mipImageLocal);

    // テクスチャリソースとSRVの作成
    text.textureResource = Dx12ResourceFactory::CreateTextureResource(device, text.metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> tempIntermediateResource = UploadTextureData(text.textureResource.Get(), text.mipImage, device, cmdList);
    intermediateUploadResources_.push_back(tempIntermediateResource);

	SRV_UAVManager::Allocation srvAllocation{};
    if (dds)
    {
        srvAllocation = srvManager->CreateSRVforDDS(text.textureResource.Get(), text.metadata);
    }
    else
    {
        srvAllocation = srvManager->CreateSRVforTexture(text.textureResource.Get(), text.metadata);
    }

	// SRVのGPUハンドルとテクスチャIDを保存
	text.textureSrvHandleGPU = srvAllocation.gpu;
	text.number = srvAllocation.index;

    // ファイルパスを保存
    text.filePath = filePath;

	pathToIDMap_[filePath] = text.number;
	if (textures_.size() <= static_cast<size_t>(text.number))
    {
		textures_.resize(text.number + 1);
    }
	textures_[text.number] = std::move(text);

	Log("成功 ID:%d", pathToIDMap_[filePath]);

    return text.number;
}

TextureData* TextureManager::GetTextureData(int32_t textureID)
{
    if (textureID < 0 || textureID >= static_cast<int32_t>(textures_.size()))
    {
        // IDが範囲外の場合はエラー
        Log("テクスチャIDが範囲外です:%d", textureID);
        assert(false);
        return nullptr;
    }
    return &textures_[textureID];
}


// 3,TextureResourceにデータを転送する
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = Dx12ResourceFactory::CreateBufferResource(device, intermediateSize);
    UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
    // Tetureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READ ResourceStateを変更する
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    commandList->ResourceBarrier(1, &barrier);
    return intermediateResource;
}

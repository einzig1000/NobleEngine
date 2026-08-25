#include "TextureLoader.h"
#include <externals/DirectXTex/d3dx12.h>
#include <externals/DirectXTex/DirectXTex.h>
#include <Utilities/Logger/Logger.h>
#include <AssetManager/Texture/TextureBank/TextureBank.h>
#include <RootBinding/StructuredBufferManager/StructuredBufferManager.h>
#include <DirectX/DirectXManager.h>
#include <filesystem>

TextureLoader::TextureLoader(DirectXManager* dxManager, TextureBank* textureBank)
	: dxManager_(dxManager), bank_(textureBank)
{}

TextureLoader::~TextureLoader()
{
	// 中間リソースの解放
	intermediateUploadResources_.clear();
}

int32_t TextureLoader::LoadTexture(const std::string & filePath)
{
	// すでに読み込まれていたらそのテクスチャIDを返す
	int32_t existingTextureID = bank_->IsTextureDataExist(filePath);
	if (existingTextureID != -1) return existingTextureID;

    Log("テクスチャ読み込み開始:%s", filePath.c_str());

    HRESULT hr = S_OK;
    std::unique_ptr<TextureData> text = std::make_unique<TextureData>();

    // ファイルパスの保存
	text->filePath = filePath;

    // 識別子を判定
    std::filesystem::path path(filePath);
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    bool dds = (ext == ".dds");

    auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
    auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);
    auto* device = dxManager_->GetDevice();
    auto* srvManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();

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
    text->metadata = mipImageLocal.GetMetadata();
    text->mipImage = std::move(mipImageLocal);

    // テクスチャリソースとSRVの作成
    text->textureResource = Dx12ResourceFactory::CreateTextureResource(device, text->metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> tempIntermediateResource = UploadTextureData(text->textureResource.Get(), text->mipImage, device, cmdList);
    intermediateUploadResources_.push_back(tempIntermediateResource);

	// SRVの作成
    SRV_UAVManager::Allocation srvAllocation{};
    if (dds)
    {
        srvAllocation = srvManager->CreateSRVforDDS(text->textureResource.Get(), text->metadata);
    }
    else
    {
        srvAllocation = srvManager->CreateSRVforTexture(text->textureResource.Get(), text->metadata);
    }

	// データ保存
	bank_->AddTextureData(filePath, srvAllocation.index, std::move(text));

    Log("成功 ID:%d", srvAllocation.index);

    return srvAllocation.index;
}

// 3,TextureResourceにデータを転送する
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureLoader::UploadTextureData(
    ID3D12Resource* texture, 
    const DirectX::ScratchImage& mipImages, 
    ID3D12Device2* device,
    ID3D12GraphicsCommandList6* commandList)
{
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	// Uploadヒープに必要なサイズを取得
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	// Uploadヒープの作成
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = 
        Dx12ResourceFactory::CreateBufferResource(device, intermediateSize);
    // コマンドリストにコピー処理を記録
    UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
    // ResourceStateをD3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READに変更する
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

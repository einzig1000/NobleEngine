#pragma once
#include <EngineDefinition/EngineDefinition.h>

class DirectXManager;
class TextureBank;
class StructuredBufferManager;

class TextureLoader
{
public:
	TextureLoader(DirectXManager* dxManager, TextureBank* textureBank);
	~TextureLoader();

	// テクスチャ読み込み
	int32_t LoadTexture(const std::string& filePath);

private:
	DirectXManager* dxManager_;
	TextureBank* bank_;

	// アップロード用一時リソースを保持するリスト
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateUploadResources_;


    // <summary>
    // テクスチャデータを GPU にアップロードする関数
    // </summary>
    // <param name="texture">アップロード先のテクスチャリソース</param>
    // <param name="mipImages">ミップマップデータ</param>
    // <param name="device">DirectX 12 デバイス</param>
    // <param name="commandList">コマンドリスト</param>
    // <returns>アップロードに使用した中間リソース</returns>
    [[nodiscard]]
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device2* device, ID3D12GraphicsCommandList6* commandList);
};

#pragma once
#include <cstdint>
#include <string>
#include <d3d12.h>
#include <vector>
#include <unordered_map>
#include <definition/definition.h>

class DirectXManager;

/// <summary>
/// テクスチャ管理クラス
/// </summary>
class TextureManager
{
public:
    TextureManager(
        DirectXManager* dxManager);
    ~TextureManager();

    // テクスチャ読み込み
    int32_t LoadTexture(const std::string& filePath);

    // データ取得
    TextureData* GetTextureData(int32_t textureID);

    // テクスチャ数を取得
    size_t GetTextureCount() const { return textures_.size(); }

private:
    DirectXManager* dxManager_ = nullptr;

    // 画像データを詰める
	std::unordered_map<std::string, int32_t> pathToIDMap_;
    std::vector<TextureData> textures_;

    // アップロード用一時リソースを保持するリスト
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateUploadResources_;

    /// <summary>
    /// テクスチャデータを GPU にアップロードする関数
    /// </summary>
    /// <param name="texture">アップロード先のテクスチャリソース</param>
    /// <param name="mipImages">ミップマップデータ</param>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="commandList">コマンドリスト</param>
    /// <returns>アップロードに使用した中間リソース</returns>
    [[nodiscard]]
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

};


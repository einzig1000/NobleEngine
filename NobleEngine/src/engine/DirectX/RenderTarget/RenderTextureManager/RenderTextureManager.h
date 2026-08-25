#pragma once
#include <memory>
#include <string>
#include <d3d12.h>
#include <Windows.h>
#include <dxgiformat.h>
#include <unordered_map>
#include <EngineDefinition/EngineDefinition.h>
#include <DirectX/RenderTarget/RenderTargetStruct.h>

class DescriptorHeapManager;

class RenderTextureManager
{
public:
	RenderTextureManager(ID3D12Device2* device, ID3D12CommandQueue* commandQueue, DescriptorHeapManager* descriptorHeapManager);
	~RenderTextureManager();

    /// <summary>
	/// レンダーテクスチャを作成する
    /// </summary>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <param name="format">テクスチャのフォーマット</param>
	/// <param name="label">テクスチャの名前</param>
	/// <param name="clearColor">クリアカラー</param>
	/// <returns>テクスチャID</returns>
	int32_t CreateRenderTarget(UINT width, UINT height, DXGI_FORMAT format, std::string label, Vector4 clearColor);

	// テクスチャ保存
    bool SaveTexture(const std::string& filePath, std::string textureName, bool color);

	bool SaveAllRenderTextures(const std::string& directoryPath, bool color);

    // データ取得
    RenderTarget* Get(int32_t textureID) const;
    RenderTarget* Get(const std::string& textureName) const;

    // リサイズ
    void ResizeAllWindowDependent(UINT newWidth, UINT newHeight);

	std::vector<std::unique_ptr<RenderTarget>>& GetRenderTargets() { return renderTargets_; }

private:
	ID3D12Device2* device_;
	DescriptorHeapManager* descriptorHeapManager_;
	ID3D12CommandQueue* commandQueue_;

	// キー：テクスチャ名　値：renderTargets_配列のインデックス
	std::unordered_map<std::string, int32_t> nameToIndexMap_;
	// キー：テクスチャID　値：renderTargets_配列のインデックス
	std::unordered_map<int32_t, int32_t> idToIndexMap_;

    // レンダーテクスチャリスト
    std::vector<std::unique_ptr<RenderTarget>> renderTargets_;
};


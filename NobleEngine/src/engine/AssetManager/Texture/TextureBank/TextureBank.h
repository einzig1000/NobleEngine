#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <unordered_map>
#include <memory>

class TextureBank
{
public:

	// テクスチャデータを追加
	void AddTextureData(const std::string& filePath, const int32_t heapSlot, std::unique_ptr<TextureData> textureData);
	// filePathが同じテクスチャデータが存在するか
	int32_t IsTextureDataExist(const std::string& filePath) const;
	// データ取得
	const TextureData* GetTextureData(int32_t textureID) const;
	// テクスチャリストを取得
	const std::unordered_map<int32_t, std::unique_ptr<TextureData>>& GetTextureMap() const { return textures_; }

private:

	// キー：テクスチャのファイルパス、値：HeapSlot
	std::unordered_map<std::string, int32_t> pathToIDMap_;
	// キー：HeapSlot、値：テクスチャデータ
	std::unordered_map<int32_t, std::unique_ptr<TextureData>> textures_;
};


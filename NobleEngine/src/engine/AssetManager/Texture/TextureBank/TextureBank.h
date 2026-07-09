#pragma once
#include <definition/definition.h>
#include <unordered_map>
#include <memory>

class TextureBank
{
public:

	// テクスチャデータを追加
	void AddTextureData(const std::string& filePath, const int32_t srvIndex, std::unique_ptr<TextureData> textureData);
	// filePathが同じテクスチャデータが存在するか
	int32_t IsTextureDataExist(const std::string& filePath) const;
	// データ取得
	TextureData* GetTextureData(int32_t textureID);
	// テクスチャリストを取得
	std::unordered_map<int32_t, std::unique_ptr<TextureData>>& GetTextureList() { return textures_; }

private:

	// キー：テクスチャのファイルパス、値：SRV上のインデックス
	std::unordered_map<std::string, int32_t> pathToIDMap_;
	// キー：SRV上のインデックス、値：テクスチャデータ
	std::unordered_map<int32_t, std::unique_ptr<TextureData>> textures_;
};


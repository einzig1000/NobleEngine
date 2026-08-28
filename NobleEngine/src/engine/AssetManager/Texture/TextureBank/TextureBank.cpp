#include "TextureBank.h"
#include <Utilities/Logger/Logger.h>

void TextureBank::AddTextureData(const std::string& filePath, const int32_t heapSlot, std::unique_ptr<TextureData> textureData)
{
	pathToIDMap_[filePath] = heapSlot;
	textures_[heapSlot] = std::move(textureData);
}

int32_t TextureBank::IsTextureDataExist(const std::string& filePath) const
{
	// すでに読み込まれていたらそのテクスチャIDを返す
	auto it = pathToIDMap_.find(filePath);
	if (it != pathToIDMap_.end())
	{
		return it->second;
	}
	return -1;
}

const TextureData* TextureBank::GetTextureData(int32_t textureID) const
{
	auto it = textures_.find(textureID);
	if (it != textures_.end())
	{
		return it->second.get();
	}
	Log("存在しないテクスチャIDが要求されました: %d", textureID);
	return nullptr;
}

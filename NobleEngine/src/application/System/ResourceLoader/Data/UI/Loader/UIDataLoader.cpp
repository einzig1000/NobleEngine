#include "UIDataLoader.h"
#include <System/ResourceLoader/Data/UI/Bank/UIDataBank.h>
#include <externals/MagicEnum/magic_enum.hpp>
#include <Utilities/Json/JsonManager.h>
#include <Game.h>

UIDataLoader::UIDataLoader(UIDataBank* bank)
	: bank_(bank)
{
	Load();
}

UIDataLoader::~UIDataLoader()
{}

void UIDataLoader::Load()
{
	for (UIElementType type : magic_enum::enum_values<UIElementType>())
	{
		if (type == UIElementType::MAX) continue;
		Load(type);
	}
}

void UIDataLoader::Load(UIElementType type)
{
	const std::string path = "assets/application/json/UIConfig.json";
	const std::string idStr = std::string(magic_enum::enum_name(type));

	size_t elementsSize = 0;
	JsonManager::Load(path, "/" + idStr + "/elementsSize", elementsSize);

	std::vector<ElementData> dataList(elementsSize);
	for (size_t i = 0; i < elementsSize; ++i)
	{
		const std::string key = "/" + idStr + "/elements/" + std::to_string(i);
		ElementData& data = dataList[i];

		JsonManager::Load(path, key + "/label", data.label);
		JsonManager::Load(path, key + "/labelIndex", data.labelIndex);
		JsonManager::Load(path, key + "/texturePath", data.texturePath);
		JsonManager::Load(path, key + "/translateRate", data.translateRate);
		JsonManager::Load(path, key + "/pivot", data.pivot);
		JsonManager::Load(path, key + "/scale", data.scale);
		JsonManager::Load(path, key + "/rotate", data.rotate);

		if (!data.texturePath.empty())
		{
			data.textureID = Game::Asset::Texture::Load(data.texturePath);
			const TextureData* textureData = Game::Asset::Texture::GetData(data.textureID);
			if (textureData)
			{
				data.textureSize = Vector2(float(textureData->metadata.width), float(textureData->metadata.height));
			}
		}
	}

	bank_->SetData(type, dataList);
}

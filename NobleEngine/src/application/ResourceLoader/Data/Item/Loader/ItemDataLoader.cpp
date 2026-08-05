#include "ItemDataLoader.h"
#include <Utilities/Json/JsonManager.h>
#include <ResourceLoader/Data/Item/Bank/ItemDataBank.h>
#include <Game.h>

ItemDataLoader::ItemDataLoader(ItemDataBank* bank)
	: bank_(bank)
{
	Load();
}

ItemDataLoader::~ItemDataLoader()
{}

void ItemDataLoader::Load()
{
	std::vector<std::string> blockKeys;
	JsonManager::Load("resources/json/BlockConfig.json", "/Keys", blockKeys);

	for (const std::string& blockIDStr : blockKeys)
	{
		auto e = magic_enum::enum_cast<BlockID>(blockIDStr);
		if (!e.has_value())
		{
			__debugbreak();
			continue;
		}

		BlockInfo info;

		info.ID = e.value();

		JsonManager::Load("resources/json/BlockConfig.json", "/" + blockIDStr + "/color", info.color);
		JsonManager::Load("resources/json/BlockConfig.json", "/" + blockIDStr + "/durability", info.durability);
		JsonManager::Load("resources/json/BlockConfig.json", "/" + blockIDStr + "/isTransparent", info.isTransparent);

		bank_->SetBlockInfo(info.ID, info);
	}

	bank_->CreateBlockInfoTable();

	std::vector<std::string> toolKeys;
	JsonManager::Load("resources/json/ToolConfig.json", "/Keys", toolKeys);

	for (const std::string& toolIDStr : toolKeys)
	{
		auto e = magic_enum::enum_cast<ToolID>(toolIDStr);
		if (!e.has_value())
		{
			__debugbreak();
			continue;
		}

		ToolInfo info;
		ToolID toolID = e.value();

		JsonManager::Load("resources/json/ToolConfig.json", "/" + toolIDStr + "/durability", info.durability);
		JsonManager::Load("resources/json/ToolConfig.json", "/" + toolIDStr + "/attackPower", info.attackPower);
		JsonManager::Load("resources/json/ToolConfig.json", "/" + toolIDStr + "/miningSpeed", info.miningSpeed);
		JsonManager::Load("resources/json/ToolConfig.json", "/" + toolIDStr + "/modelID", info.modelPath);
		info.modelID = Game::Asset::Model::Load(info.modelPath);
		info.aabb = Game::Asset::Model::GetData(info.modelID)->aabb;
		JsonManager::Load("resources/json/ToolConfig.json", "/" + toolIDStr + "/textureID", info.texturePath);
		info.textureID = Game::Asset::Texture::Load(info.texturePath);
	
		bank_->SetToolInfo(toolID, info);
	}


	std::vector<std::string> itemKeys;
	JsonManager::Load("resources/json/ItemConfig.json", "/Keys", itemKeys);

	for (const std::string& itemIDStr : itemKeys)
	{
		auto e = magic_enum::enum_cast<ItemID>(itemIDStr);
		if (!e.has_value())
		{
			__debugbreak();
			continue;
		}

		ItemInfo info;
		info.id = e.value();

		std::string itemGenreStr;
		std::string blockIDStr;
		std::string toolIDStr;
		std::string objectIDStr;

		JsonManager::Load("resources/json/ItemConfig.json", "/" + itemIDStr + "/itemGenre", itemGenreStr);
		JsonManager::Load("resources/json/ItemConfig.json", "/" + itemIDStr + "/blockID", blockIDStr);
		JsonManager::Load("resources/json/ItemConfig.json", "/" + itemIDStr + "/toolID", toolIDStr);
		JsonManager::Load("resources/json/ItemConfig.json", "/" + itemIDStr + "/objectID", objectIDStr);

		auto genreEnum = magic_enum::enum_cast<ItemGenre>(itemGenreStr);
		if (!genreEnum.has_value())
		{
			info.genre = ItemGenre::MAX;
		}
		else
		{
			info.genre = genreEnum.value();
		}

		auto blockEnum = magic_enum::enum_cast<BlockID>(blockIDStr);
		if (!blockEnum.has_value())
		{
			__debugbreak();
			continue;
		}
		info.blockID = blockEnum.value();

		auto objectEnum = magic_enum::enum_cast<ObjectID>(objectIDStr);
		if (!objectEnum.has_value())
		{
			__debugbreak();
			continue;
		}
		info.objectID = objectEnum.value();

		auto toolEnum = magic_enum::enum_cast<ToolID>(toolIDStr);
		if (!toolEnum.has_value())
		{
			__debugbreak();
			continue;
		}
		info.toolID = toolEnum.value();

		bank_->SetItemInfo(info.id, info);
	}
}

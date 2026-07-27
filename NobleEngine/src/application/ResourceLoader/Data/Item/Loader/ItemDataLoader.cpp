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
		BlockInfo info;
		info.ID = StringToEnum<BlockID>(blockIDStr);

		JsonManager::Load("resources/json/BlockConfig.json", "/" + blockIDStr + "/color", info.color);
		JsonManager::Load("resources/json/BlockConfig.json", "/" + blockIDStr + "/durability", info.durability);
		JsonManager::Load("resources/json/BlockConfig.json", "/" + blockIDStr + "/isTransparent", info.isTransparent);

		bank_->SetBlockInfo(info.ID, info);
	}

	bank_->CreateBlockInfoTable();

	std::vector<std::string> toolKeys;
	JsonManager::Load("resources/json/ToolConfig.json", "/Keys", toolKeys);

	//for (const std::string& toolIDStr : toolKeys)
	//{
	//	ToolInfo info;
	//	ToolID toolID = StringToEnum<ToolID>(toolIDStr);
	//
	//	JsonManager::Load("resources/json/ToolConfig.json", "/" + toolIDStr + "/durability", info.durability);
	//	JsonManager::Load("resources/json/ToolConfig.json", "/" + toolIDStr + "/attackPower", info.attackPower);
	//	JsonManager::Load("resources/json/ToolConfig.json", "/" + toolIDStr + "/miningSpeed", info.miningSpeed);
	//	JsonManager::Load("resources/json/ToolConfig.json", "/" + toolIDStr + "/modelPath", info.modelPath);
	//	info.modelID = Game::Asset::Model::Load(info.modelPath);
	//	info.aabb = Game::Asset::Model::GetData(info.modelID)->aabb;
	//	JsonManager::Load("resources/json/ToolConfig.json", "/" + toolIDStr + "/texturePath", info.texturePath);
	//	info.textureID = Game::Asset::Texture::Load(info.texturePath);
	//
	//	bank_->SetToolInfo(toolID, info);
	//}


	std::vector<std::string> itemKeys;
	JsonManager::Load("resources/json/ItemConfig.json", "/Keys", itemKeys);

	for (const std::string& itemIDStr : itemKeys)
	{
		ItemInfo info;
		info.id = StringToEnum<ItemID>(itemIDStr);

		std::string itemGenreStr;
		std::string blockIDStr;
		std::string toolIDStr;
		std::string objectIDStr;

		JsonManager::Load("resources/json/ItemConfig.json", "/" + itemIDStr + "/itemGenre", itemGenreStr);
		JsonManager::Load("resources/json/ItemConfig.json", "/" + itemIDStr + "/blockID", blockIDStr);
		JsonManager::Load("resources/json/ItemConfig.json", "/" + itemIDStr + "/toolID", toolIDStr);
		JsonManager::Load("resources/json/ItemConfig.json", "/" + itemIDStr + "/objectID", objectIDStr);

		info.genre = StringToEnum<ItemGenre>(itemGenreStr);
		info.blockID = StringToEnum<BlockID>(blockIDStr);
		info.objectID = StringToEnum<ObjectID>(objectIDStr);
		info.toolID = StringToEnum<ToolID>(toolIDStr);

		bank_->SetItemInfo(info.id, info);
	}
}

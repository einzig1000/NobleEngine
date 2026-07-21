#include "ItemConfig.h"
#include <Utilities/Json/JsonManager.h>
#include <Game.h>

ItemConfig::ItemConfig()
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

		blockInfoMap_[info.ID] = info;
	}


	std::vector<std::string> toolKeys;
	JsonManager::Load("resources/json/ToolConfig.json", "/Keys", toolKeys);

	for (const std::string& toolIDStr : toolKeys)
	{
		ToolInfo info;
		ToolID toolID = StringToEnum<ToolID>(toolIDStr);

		JsonManager::Load("resources/json/ToolConfig.json", "/" + toolIDStr + "/durability", info.durability);
		JsonManager::Load("resources/json/ToolConfig.json", "/" + toolIDStr + "/attackPower", info.attackPower);
		JsonManager::Load("resources/json/ToolConfig.json", "/" + toolIDStr + "/miningSpeed", info.miningSpeed);

		toolInfoMap_[toolID] = info;
	}


	std::vector<std::string> itemKeys;
	JsonManager::Load("resources/json/ItemConfig.json", "/Keys", itemKeys);

	for (const std::string& itemIDStr : itemKeys)
	{
		ItemInfo info;
		info.id = StringToEnum<ItemID>(itemIDStr);

		std::string itemGenreStr;
		std::string texturePath;
		std::string modelPath;

		JsonManager::Load("resources/json/ItemConfig.json", "/" + itemIDStr + "/itemGenre", itemGenreStr);
		JsonManager::Load("resources/json/ItemConfig.json", "/" + itemIDStr + "/texturePath", texturePath);
		JsonManager::Load("resources/json/ItemConfig.json", "/" + itemIDStr + "/modelPath", modelPath);

		info.genre = StringToEnum<ItemGenre>(itemGenreStr);
		info.textureID = Game::Asset::Texture::Load(texturePath);
		info.modelID = Game::Asset::Model::Load(modelPath);
		info.aabb = Game::Asset::Model::GetData(info.modelID)->aabb;

		if (info.genre == ItemGenre::Block)
		{
			std::string blockIDStr;
			JsonManager::Load("resources/json/ItemConfig.json", "/" + itemIDStr + "/blockID", blockIDStr);
			BlockID blockID = StringToEnum<BlockID>(blockIDStr);
			info.blockInfo = blockInfoMap_[blockID];
		}
		else if (info.genre == ItemGenre::Tool)
		{
			std::string toolIDStr;
			JsonManager::Load("resources/json/ItemConfig.json", "/" + itemIDStr + "/toolID", toolIDStr);
			ToolID toolID = StringToEnum<ToolID>(toolIDStr);
			info.toolInfo = toolInfoMap_[toolID];
		}

		itemInfoMap_[info.id] = info;
	}
}

ItemConfig::~ItemConfig()
{}

ItemConfig& ItemConfig::Instance()
{
	static ItemConfig instance;
	return instance;
}

const ItemInfo& ItemConfig::GetItemInfo(ItemID id)
{
	return itemInfoMap_.at(id);
}

const BlockInfo& ItemConfig::GetBlockInfo(BlockID id)
{
	return blockInfoMap_.at(id);
}

const ToolInfo& ItemConfig::GetToolInfo(ToolID id)
{
	return toolInfoMap_.at(id);
}

#include "ItemDataSaver.h"
#include <Utilities/Json/JsonManager.h>
#include <ResourceLoader/Data/Item/Bank/ItemDataBank.h>
#include <Game.h>

ItemDataSaver::ItemDataSaver(ItemDataBank* bank)
	: bank_(bank)
{
}

ItemDataSaver::~ItemDataSaver()
{
}

void ItemDataSaver::Save(BlockID id, const BlockInfo& info)
{
	bank_->SetBlockInfo(id, info);

	const std::string path = "assets/application/json/BlockConfig.json";
	const std::string idStr = std::string(magic_enum::enum_name(id));

	JsonManager::AddParam(path, "/" + idStr + "/color", info.color);
	JsonManager::AddParam(path, "/" + idStr + "/durability", info.durability);
	JsonManager::AddParam(path, "/" + idStr + "/isTransparent", info.isTransparent);

	JsonManager::Save(path);
}

void ItemDataSaver::Save(ToolID id, const ToolInfo& info)
{
	bank_->SetToolInfo(id, info);

	const std::string path = "assets/application/json/ToolConfig.json";
	const std::string idStr = std::string(magic_enum::enum_name(id));

	JsonManager::AddParam(path, "/" + idStr + "/durability", info.durability);
	JsonManager::AddParam(path, "/" + idStr + "/attackPower", info.attackPower);
	JsonManager::AddParam(path, "/" + idStr + "/miningSpeed", info.miningSpeed);

	JsonManager::Save(path);
}

void ItemDataSaver::Save(ItemID id, const ItemInfo& info)
{
	bank_->SetItemInfo(id, info);

	const std::string path = "assets/application/json/ItemConfig.json";
	const std::string idStr = std::string(magic_enum::enum_name(id));
	const std::string modelPath = Game::Asset::Model::GetData(info.modelID)->filePath;
	const std::string texturePath = Game::Asset::Texture::GetData(info.textureID)->filePath;

	JsonManager::AddParam(path, "/" + idStr + "/itemGenre", magic_enum::enum_name(info.genre).data());
	JsonManager::AddParam(path, "/" + idStr + "/blockID", magic_enum::enum_name(info.blockID).data());
	JsonManager::AddParam(path, "/" + idStr + "/toolID", magic_enum::enum_name(info.toolID).data());
	JsonManager::AddParam(path, "/" + idStr + "/objectID", magic_enum::enum_name(info.objectID).data());
	JsonManager::AddParam(path, "/" + idStr + "/modelPath", modelPath);
	JsonManager::AddParam(path, "/" + idStr + "/texturePath", texturePath);

	JsonManager::Save(path);
}

void ItemDataSaver::Save(ObjectID id, const ObjectInfo& info)
{
	bank_->SetObjectInfo(id, info);

}
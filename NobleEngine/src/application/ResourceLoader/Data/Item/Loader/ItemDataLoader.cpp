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
	// ブロック情報の読み込み
	for (BlockID id : magic_enum::enum_values<BlockID>())
	{
		if (id == BlockID::MAX) continue;
		Load(id);
	}
	bank_->CreateBlockInfoTable();

	// ツール情報の読み込み
	for (ToolID id : magic_enum::enum_values<ToolID>())
	{
		if (id == ToolID::MAX) continue;
		Load(id);
	}

	// アイテム情報の読み込み
	for (ItemID id : magic_enum::enum_values<ItemID>())
	{
		if (id == ItemID::MAX) continue;
		Load(id);
	}

	// オブジェクト情報の読み込み
	for (ObjectID id : magic_enum::enum_values<ObjectID>())
	{
		if (id == ObjectID::MAX) continue;
		Load(id);
	}

	//// ブロック情報の読み込み
	//std::vector<std::string> blockKeys;
	//JsonManager::Load("assets/application/json/BlockConfig.json", "/Keys", blockKeys);
	// 
	//for (const std::string& blockIDStr : blockKeys)
	//{
	//	auto e = magic_enum::enum_cast<BlockID>(blockIDStr);
	//	if (!e.has_value())
	//	{
	//		__debugbreak();
	//		continue;
	//	}
	// 
	//	BlockInfo info;
	// 
	//	info.ID = e.value();
	// 
	//	JsonManager::Load("assets/application/json/BlockConfig.json", "/" + blockIDStr + "/color", info.color);
	//	JsonManager::Load("assets/application/json/BlockConfig.json", "/" + blockIDStr + "/durability", info.durability);
	//	JsonManager::Load("assets/application/json/BlockConfig.json", "/" + blockIDStr + "/isTransparent", info.isTransparent);
	// 
	//	bank_->SetBlockInfo(info.ID, info);
	//}
	// 
	//bank_->CreateBlockInfoTable();
	// 
	// 
	//// ツール情報の読み込み
	//std::vector<std::string> toolKeys;
	//JsonManager::Load("assets/application/json/ToolConfig.json", "/Keys", toolKeys);
	// 
	//for (const std::string& toolIDStr : toolKeys)
	//{
	//	auto e = magic_enum::enum_cast<ToolID>(toolIDStr);
	//	if (!e.has_value())
	//	{
	//		__debugbreak();
	//		continue;
	//	}
	// 
	//	ToolInfo info;
	//	ToolID toolID = e.value();
	// 
	//	JsonManager::Load("assets/application/json/ToolConfig.json", "/" + toolIDStr + "/durability", info.durability);
	//	JsonManager::Load("assets/application/json/ToolConfig.json", "/" + toolIDStr + "/attackPower", info.attackPower);
	//	JsonManager::Load("assets/application/json/ToolConfig.json", "/" + toolIDStr + "/miningSpeed", info.miningSpeed);
	//	//JsonManager::Load("assets/application/json/ToolConfig.json", "/" + toolIDStr + "/modelID", info.modelPath);
	//	//info.modelID = Game::Asset::Model::Load(info.modelPath);
	//	//info.colliderShape = Game::Asset::Model::GetData(info.modelID)->colliderShape;
	//	//JsonManager::Load("assets/application/json/ToolConfig.json", "/" + toolIDStr + "/textureID", info.texturePath);
	//	//info.textureID = Game::Asset::Texture::Load(info.texturePath);
	//
	//	bank_->SetToolInfo(toolID, info);
	//}
	// 
	// 
	//// アイテム情報の読み込み
	//std::vector<std::string> itemKeys;
	//JsonManager::Load("assets/application/json/ItemConfig.json", "/Keys", itemKeys);
	// 
	//for (const std::string& itemIDStr : itemKeys)
	//{
	//	auto e = magic_enum::enum_cast<ItemID>(itemIDStr);
	//	if (!e.has_value())
	//	{
	//		__debugbreak();
	//		continue;
	//	}
	// 
	//	ItemInfo info;
	//	info.id = e.value();
	// 
	//	std::string itemGenreStr;
	//	std::string blockIDStr;
	//	std::string toolIDStr;
	//	std::string objectIDStr;
	// 
	//	JsonManager::Load("assets/application/json/ItemConfig.json", "/" + itemIDStr + "/itemGenre", itemGenreStr);
	//	JsonManager::Load("assets/application/json/ItemConfig.json", "/" + itemIDStr + "/blockID", blockIDStr);
	//	JsonManager::Load("assets/application/json/ItemConfig.json", "/" + itemIDStr + "/toolID", toolIDStr);
	//	JsonManager::Load("assets/application/json/ItemConfig.json", "/" + itemIDStr + "/objectID", objectIDStr);
	//	JsonManager::Load("assets/application/json/ItemConfig.json", "/" + itemIDStr + "/modelPath", info.modelPath);
	//	info.modelID = Game::Asset::Model::Load(info.modelPath);
	//	JsonManager::Load("assets/application/json/ItemConfig.json", "/" + itemIDStr + "/texturePath", info.texturePath);
	//	info.textureID = Game::Asset::Texture::Load(info.texturePath);
	// 
	//	auto genreEnum = magic_enum::enum_cast<ItemGenre>(itemGenreStr);
	//	if (!genreEnum.has_value())
	//	{
	//		info.genre = ItemGenre::MAX;
	//	}
	//	else
	//	{
	//		info.genre = genreEnum.value();
	//	}
	// 
	//	auto blockEnum = magic_enum::enum_cast<BlockID>(blockIDStr);
	//	if (!blockEnum.has_value())
	//	{
	//		__debugbreak();
	//		continue;
	//	}
	//	info.blockID = blockEnum.value();
	// 
	//	auto objectEnum = magic_enum::enum_cast<ObjectID>(objectIDStr);
	//	if (!objectEnum.has_value())
	//	{
	//		__debugbreak();
	//		continue;
	//	}
	//	info.objectID = objectEnum.value();
	// 
	//	auto toolEnum = magic_enum::enum_cast<ToolID>(toolIDStr);
	//	if (!toolEnum.has_value())
	//	{
	//		__debugbreak();
	//		continue;
	//	}
	//	info.toolID = toolEnum.value();
	// 
	//	bank_->SetItemInfo(info.id, info);
	//}
}

void ItemDataLoader::Load(ToolID id)
{
	const std::string path = "assets/application/json/ToolConfig.json";
	const std::string idStr = std::string(magic_enum::enum_name(id));

	ToolInfo info;
	info.ID = id;

	JsonManager::Load(path, "/" + idStr + "/durability", info.durability);
	JsonManager::Load(path, "/" + idStr + "/attackPower", info.attackPower);
	JsonManager::Load(path, "/" + idStr + "/miningSpeed", info.miningSpeed);

	bank_->SetToolInfo(id, info);
}

void ItemDataLoader::Load(BlockID id)
{
	const std::string path = "assets/application/json/BlockConfig.json";
	const std::string idStr = std::string(magic_enum::enum_name(id));

	BlockInfo info;
	info.ID = id;

	JsonManager::Load(path, "/" + idStr + "/color", info.color);
	JsonManager::Load(path, "/" + idStr + "/durability", info.durability);
	JsonManager::Load(path, "/" + idStr + "/isTransparent", info.isTransparent);

	bank_->SetBlockInfo(id, info);
}

void ItemDataLoader::Load(ObjectID id)
{
	ObjectInfo info;
	info.ID = id;

	bank_->SetObjectInfo(id, info);
}

void ItemDataLoader::Load(ItemID id)
{
	const std::string path = "assets/application/json/ItemConfig.json";
	const std::string idStr = std::string(magic_enum::enum_name(id));

	ItemInfo info;
	info.id = id;

	std::string itemGenreStr;
	std::string blockIDStr;
	std::string toolIDStr;
	std::string objectIDStr;
	std::string modelPath;
	std::string texturePath;

	JsonManager::Load(path, "/" + idStr + "/itemGenre", itemGenreStr);
	JsonManager::Load(path, "/" + idStr + "/blockID", blockIDStr);
	JsonManager::Load(path, "/" + idStr + "/toolID", toolIDStr);
	JsonManager::Load(path, "/" + idStr + "/objectID", objectIDStr);
	JsonManager::Load(path, "/" + idStr + "/modelPath", modelPath);
	info.modelID = Game::Asset::Model::Load(modelPath);
	JsonManager::Load(path, "/" + idStr + "/texturePath", texturePath);
	info.textureID = Game::Asset::Texture::Load(texturePath);

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
		return;
	}
	info.blockID = blockEnum.value();

	auto toolEnum = magic_enum::enum_cast<ToolID>(toolIDStr);
	if (!toolEnum.has_value())
	{
		__debugbreak();
		return;
	}
	info.toolID = toolEnum.value();

	auto objectEnum = magic_enum::enum_cast<ObjectID>(objectIDStr);
	if (!objectEnum.has_value())
	{
		__debugbreak();
		return;
	}
	info.objectID = objectEnum.value();

	bank_->SetItemInfo(id, info);
}
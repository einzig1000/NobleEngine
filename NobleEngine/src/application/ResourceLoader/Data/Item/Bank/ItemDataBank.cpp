#include "ItemDataBank.h"
#include <Game.h>

ItemDataBank::ItemDataBank()
{}

ItemDataBank::~ItemDataBank()
{}

void ItemDataBank::SetItemInfo(ItemID id, const ItemInfo& info)
{
	itemInfoMap_[id] = info;
}

void ItemDataBank::SetBlockInfo(BlockID id, const BlockInfo& info)
{
	blockInfoMap_[id] = info;
}

void ItemDataBank::SetToolInfo(ToolID id, const ToolInfo& info)
{
	toolInfoMap_[id] = info;
}

void ItemDataBank::SetObjectInfo(ObjectID id, const ObjectInfo& info)
{
	objectInfoMap_[id] = info;
}

void ItemDataBank::CreateBlockInfoTable()
{
	blockInfoTable_.clear();
	blockInfoTable_.reserve(blockInfoMap_.size());
	for (const auto& pair : blockInfoMap_)
	{
		const BlockInfo& info = pair.second;
		Vector4uint tableElement;
		tableElement.x = info.color;			// x = 16進数color
		tableElement.y = info.isTransparent;	// y = 透過ブロックかどうか
		tableElement.z = 0; tableElement.w = 0;	// z, w = 0
		blockInfoTable_.emplace_back(tableElement);
	}

	int32_t blockInfoTableResourceID_ = Game::Resource::CreateStatic(blockInfoTable_);
	blockInfoTableHeapSlot_ = Game::Resource::GetSRV(blockInfoTableResourceID_);
}

const ItemInfo* ItemDataBank::GetItemInfo(ItemID id)
{
	auto it = itemInfoMap_.find(id);
	if (it != itemInfoMap_.end())
	{
		return &(it->second);
	}
	return nullptr;
}

const BlockInfo* ItemDataBank::GetBlockInfo(BlockID id)
{
	auto it = blockInfoMap_.find(id);
	if (it != blockInfoMap_.end())
	{
		return &(it->second);
	}
	return nullptr;
}

const ToolInfo* ItemDataBank::GetToolInfo(ToolID id)
{
	auto it = toolInfoMap_.find(id);
	if (it != toolInfoMap_.end())
	{
		return &(it->second);
	}
	return nullptr;
}

const ObjectInfo* ItemDataBank::GetObjectInfo(ObjectID id)
{
	auto it = objectInfoMap_.find(id);
	if (it != objectInfoMap_.end())
	{
		return &(it->second);
	}
	return nullptr;
}
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

	blockInfoTableSrvIndex_ = Game::Resource::CreateStatic(blockInfoTable_);
}

const ItemInfo& ItemDataBank::GetItemInfo(ItemID id)
{
	return itemInfoMap_.at(id);
}

const BlockInfo& ItemDataBank::GetBlockInfo(BlockID id)
{
	return blockInfoMap_.at(id);
}

const ToolInfo& ItemDataBank::GetToolInfo(ToolID id)
{
	return toolInfoMap_.at(id);
}

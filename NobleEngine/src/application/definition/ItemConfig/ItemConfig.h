#pragma once
#include <definition/definition.h>
#include <unordered_map>

class ItemConfig
{
public:
	static ItemConfig& Instance();

	// コピー・ムーブ禁止
	ItemConfig(const ItemConfig&) = delete;
	ItemConfig& operator=(const ItemConfig&) = delete;
	ItemConfig(ItemConfig&&) = delete;
	ItemConfig& operator=(ItemConfig&&) = delete;

	const ItemInfo& GetItemInfo(ItemID id);
	const BlockInfo& GetBlockInfo(BlockID id);
	const ToolInfo& GetToolInfo(ToolID id);

private:
	ItemConfig();
	~ItemConfig();


	std::unordered_map<ItemID, ItemInfo> itemInfoMap_;
	std::unordered_map<BlockID, BlockInfo> blockInfoMap_;
	std::unordered_map<ToolID, ToolInfo> toolInfoMap_;
};


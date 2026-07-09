#pragma once
#include <definition/definition.h>
#include <unordered_map>

class ItemConfig
{
public:
	ItemConfig();
	~ItemConfig();

private:
	std::unordered_map<ItemID, ItemInfo> itemInfoMap_;
	std::unordered_map<BlockID, BlockInfo> blockInfoMap_;
};


#pragma once
#include <definition/definition.h>
#include <unordered_map>

class ItemDataBank
{
public:
	ItemDataBank();
	~ItemDataBank();

	void SetItemInfo(ItemID id, const ItemInfo& info);
	void SetBlockInfo(BlockID id, const BlockInfo& info);
	void SetToolInfo(ToolID id, const ToolInfo& info);
	void SetObjectInfo(ObjectID id, const ObjectInfo& info);
	void CreateBlockInfoTable();

	const ItemInfo* GetItemInfo(ItemID id);
	const BlockInfo* GetBlockInfo(BlockID id);
	const ToolInfo* GetToolInfo(ToolID id);
	const ObjectInfo* GetObjectInfo(ObjectID id);
	const int32_t GetBlockInfoTableHeapSlot() { return blockInfoTableHeapSlot_; }

private:
	std::unordered_map<ItemID, ItemInfo> itemInfoMap_;
	std::unordered_map<BlockID, BlockInfo> blockInfoMap_;
	std::vector<Vector4uint> blockInfoTable_;
	std::unordered_map<ToolID, ToolInfo> toolInfoMap_;
	std::unordered_map<ObjectID, ObjectInfo> objectInfoMap_;

	int32_t blockInfoTableHeapSlot_ = -1;
};


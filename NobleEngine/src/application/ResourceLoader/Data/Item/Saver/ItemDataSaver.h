#pragma once
#include <definition/definition.h>

class ItemDataBank;

class ItemDataSaver
{
public:
	ItemDataSaver(ItemDataBank* bank);
	~ItemDataSaver();

	void Save(BlockID id, const BlockInfo& info);
	void Save(ToolID id, const ToolInfo& info);
	void Save(ItemID id, const ItemInfo& info);
	void Save(ObjectID id, const ObjectInfo& info);

private:
	ItemDataBank* bank_;
};
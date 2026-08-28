#pragma once
#include <definition/definition.h>
#include <unordered_map>

class ItemDataBank;

class ItemDataLoader
{
public:
	ItemDataLoader(ItemDataBank* bank);
	~ItemDataLoader();

	// フルロード
	void Load();

	// ID単位の個別ロード
	void Load(ToolID id);
	void Load(ItemID id);
	void Load(BlockID id);
	void Load(ObjectID id);

private:

	ItemDataBank* bank_;

};


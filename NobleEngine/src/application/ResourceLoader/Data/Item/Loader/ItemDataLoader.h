#pragma once
#include <definition/definition.h>
#include <unordered_map>

class ItemDataBank;

class ItemDataLoader
{
public:
	ItemDataLoader(ItemDataBank* bank);
	~ItemDataLoader();

	void Load();

private:
	ItemDataBank* bank_;

};


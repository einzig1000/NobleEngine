#pragma once
#include "Item/ItemDataManager.h"
#include <memory>

class DataManager
{
public:
	DataManager();
	~DataManager();


	ItemDataManager* const GetItemDataManager() { return itemDataManager_.get(); }

private:
	std::unique_ptr<ItemDataManager> itemDataManager_;


	bool loaded_ = false;
};


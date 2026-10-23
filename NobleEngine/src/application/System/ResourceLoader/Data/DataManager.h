#pragma once
#include "Item/ItemDataManager.h"
#include "UI/UIDataManager.h"
#include <memory>

class DataManager
{
public:
	DataManager();
	~DataManager();


	ItemDataManager* const GetItemDataManager() { return itemDataManager_.get(); }
	UIDataManager* const GetUIDataManager() { return uiDataManager_.get(); }


private:
	std::unique_ptr<ItemDataManager> itemDataManager_;
	std::unique_ptr<UIDataManager> uiDataManager_;


	bool loaded_ = false;
};


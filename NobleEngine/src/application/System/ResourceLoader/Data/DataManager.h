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
	UIDataManager* const GetUIElementDataManager() { return uiElementDataManager_.get(); }


private:
	std::unique_ptr<ItemDataManager> itemDataManager_;
	std::unique_ptr<UIDataManager> uiElementDataManager_;


	bool loaded_ = false;
};


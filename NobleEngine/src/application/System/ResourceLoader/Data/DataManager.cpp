#include "DataManager.h"

DataManager::DataManager()
{
	itemDataManager_ = std::make_unique<ItemDataManager>();
	uiDataManager_ = std::make_unique<UIDataManager>();
}

DataManager::~DataManager()
{}

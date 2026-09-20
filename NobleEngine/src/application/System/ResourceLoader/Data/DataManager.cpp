#include "DataManager.h"

DataManager::DataManager()
{
	itemDataManager_ = std::make_unique<ItemDataManager>();
}

DataManager::~DataManager()
{}

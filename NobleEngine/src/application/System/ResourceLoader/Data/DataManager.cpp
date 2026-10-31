#include "DataManager.h"

DataManager::DataManager()
{
	itemDataManager_ = std::make_unique<ItemDataManager>();
	uiDataManager_ = std::make_unique<UIDataManager>();
	skillTreeDataManager_ = std::make_unique<SkillTreeDataManager>();
}

DataManager::~DataManager()
{}

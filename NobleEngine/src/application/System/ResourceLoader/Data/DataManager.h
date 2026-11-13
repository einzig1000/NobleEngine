#pragma once
#include "Item/ItemDataManager.h"
#include "UI/UIDataManager.h"
#include "SkillTree/SkillTreeDataManager.h"
#include <memory>

class DataManager
{
public:
	DataManager();
	~DataManager();


	ItemDataManager* const GetItemDataManager() { return itemDataManager_.get(); }
	UIDataManager* const GetUIDataManager() { return uiDataManager_.get(); }
	SkillTreeDataManager* const GetSkillTreeDataManager() { return skillTreeDataManager_.get(); }


private:
	std::unique_ptr<ItemDataManager> itemDataManager_;
	std::unique_ptr<UIDataManager> uiDataManager_;
	std::unique_ptr<SkillTreeDataManager> skillTreeDataManager_;

};

#include "SkillTreeDataManager.h"

SkillTreeDataManager::SkillTreeDataManager()
{
	bank_ = std::make_unique<SkillTreeDataBank>();

	loader_ = std::make_unique<SkillTreeDataLoader>(bank_.get());

	saver_ = std::make_unique<SkillTreeDataSaver>(bank_.get());
}

SkillTreeDataManager::~SkillTreeDataManager()
{}

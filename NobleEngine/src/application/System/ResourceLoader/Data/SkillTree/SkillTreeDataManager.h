#pragma once
#include "Bank/SkillTreeDataBank.h"
#include "Loader/SkillTreeDataLoader.h"
#include "Saver/SkillTreeDataSaver.h"
#include <memory>

class SkillTreeDataManager
{
public:
	SkillTreeDataManager();
	~SkillTreeDataManager();

	SkillTreeDataBank* GetBank() const { return bank_.get(); }
	SkillTreeDataLoader* GetLoader() const { return loader_.get(); }
	SkillTreeDataSaver* GetSaver() const { return saver_.get(); }

private:
	std::unique_ptr<SkillTreeDataBank> bank_;
	std::unique_ptr<SkillTreeDataLoader> loader_;
	std::unique_ptr<SkillTreeDataSaver> saver_;
};

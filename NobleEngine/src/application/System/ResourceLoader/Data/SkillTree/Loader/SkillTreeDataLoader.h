#pragma once
#include <definition/SkillTreeData/SkillTreeData.h>

class SkillTreeDataBank;

class SkillTreeDataLoader
{
public:
	SkillTreeDataLoader(SkillTreeDataBank* bank);
	~SkillTreeDataLoader();

	void Load();

private:
	SkillTreeDataBank* bank_;
};
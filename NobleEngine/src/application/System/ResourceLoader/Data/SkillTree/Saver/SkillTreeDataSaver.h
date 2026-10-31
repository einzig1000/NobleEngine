#pragma once
#include <definition/SkillTreeData/SkillTreeData.h>
#include <vector>

class SkillTreeDataBank;

class SkillTreeDataSaver
{
public:
	SkillTreeDataSaver(SkillTreeDataBank* bank);
	~SkillTreeDataSaver();

	void Save(const std::vector<SkillNodeData>& nodes);

private:
	SkillTreeDataBank* bank_;
};
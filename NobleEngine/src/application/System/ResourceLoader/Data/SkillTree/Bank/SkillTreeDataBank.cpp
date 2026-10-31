#include "SkillTreeDataBank.h"

void SkillTreeDataBank::SetNodes(const std::vector<SkillNodeData>& nodes)
{
	nodes_ = nodes;
	++revision_;
}

const SkillNodeData* SkillTreeDataBank::Find(int32_t id) const
{
	for (const SkillNodeData& node : nodes_)
	{
		if (node.id == id) return &node;
	}
	return nullptr;
}
#pragma once
#include <definition/SkillTreeData/SkillTreeData.h>
#include <vector>

class SkillTreeDataBank
{
public:
	void SetNodes(const std::vector<SkillNodeData>& nodes);

	const std::vector<SkillNodeData>& GetNodes() const { return nodes_; }
	// 番号からノードを探す。無ければ nullptr
	const SkillNodeData* Find(int32_t id) const;
	// SetNodes のたびに1増える(UIが作り直すかどうかの判定に使う)
	int32_t GetRevision() const { return revision_; }

private:
	std::vector<SkillNodeData> nodes_;
	int32_t revision_ = 0;
};
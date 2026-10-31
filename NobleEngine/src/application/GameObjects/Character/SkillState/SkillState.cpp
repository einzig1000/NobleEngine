#include "SkillState.h"
#include <GameObjects/Character/MiningPointGauge/MiningPointGauge.h>
#include <GameObjects/Character/ItemInventory/ItemInventory.h>

void SkillState::Reset()
{
	unlocked_.clear();
	totals_.fill(0.0f);
}

bool SkillState::AreParentsUnlocked(const SkillNodeData& node) const
{
	for (int32_t parentID : node.parents)
	{
		if (!IsUnlocked(parentID)) return false;
	}
	return true;
}

SkillNodeStatus SkillState::GetStatus(const SkillNodeData& node, const MiningPointGauge* gauge, const ItemInventory* inventory) const
{
	if (IsUnlocked(node.id)) return SkillNodeStatus::Unlocked;
	if (!AreParentsUnlocked(node)) return SkillNodeStatus::Locked;

	// レベル
	if (gauge && gauge->GetLevel() < node.levelCost) return SkillNodeStatus::NotEnoughCost;

	// 素材
	if (inventory)
	{
		for (const SkillCostItem& cost : node.itemCosts)
		{
			if (inventory->CountItem(cost.id) < cost.count) return SkillNodeStatus::NotEnoughCost;
		}
	}

	return SkillNodeStatus::Unlockable;
}

void SkillState::Unlock(const SkillNodeData& node)
{
	// 2回目は何もしない(効果が2重にならないように)
	if (!unlocked_.insert(node.id).second) return;

	// 機能は「1以上なら解放」、数値は値を足す
	const float add = IsFeatureEffect(node.effectType) ? 1.0f : node.effectValue;
	totals_[static_cast<size_t>(node.effectType)] += add;
}
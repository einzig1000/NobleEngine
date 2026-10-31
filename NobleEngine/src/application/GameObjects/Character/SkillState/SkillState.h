#pragma once
#include <definition/SkillTreeData/SkillTreeData.h>
#include <externals/MagicEnum/magic_enum.hpp>
#include <array>
#include <unordered_set>

class MiningPointGauge;
class ItemInventory;

// ノードの状態(UIの見え方と、開放できるかの判定に使う)
enum class SkillNodeStatus
{
	Unlocked,		// 開放済み
	Unlockable,		// 前提は開放済みで、今すぐ開放できる
	NotEnoughCost,	// 前提は開放済みだが、レベルか素材が足りない
	Locked,			// 前提が開放されていない
};

/// <summary>
/// スキルツリーの開放状況の管理(Playerが持つ。UIは読み取り専用で見る)
/// </summary>
class SkillState
{
public:
	// 全部未開放に戻す
	void Reset();

	bool IsUnlocked(int32_t nodeID) const { return unlocked_.contains(nodeID); }
	// 前提(親)が全部開放済みか
	bool AreParentsUnlocked(const SkillNodeData& node) const;
	// ノードの状態。gauge・inventory が nullptr のときはコストが足りている扱い
	SkillNodeStatus GetStatus(const SkillNodeData& node, const MiningPointGauge* gauge, const ItemInventory* inventory) const;

	// 開放済みにして効果を足す(状態のチェックとコストの支払いは呼ぶ側)
	void Unlock(const SkillNodeData& node);

	// 効果の合計(開放済みノードの effectValue を種類ごとに足したもの)
	float GetTotal(SkillEffectType type) const { return totals_[static_cast<size_t>(type)]; }
	// 機能が解放されているか
	bool HasFeature(SkillEffectType type) const { return GetTotal(type) > 0.0f; }

	size_t GetUnlockedCount() const { return unlocked_.size(); }

private:
	std::unordered_set<int32_t> unlocked_;
	std::array<float, magic_enum::enum_count<SkillEffectType>()> totals_{};
};
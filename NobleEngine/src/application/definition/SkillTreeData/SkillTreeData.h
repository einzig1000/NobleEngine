#pragma once
#include <definition/definition.h>
#include <cstdint>
#include <string>
#include <vector>

// スキルの効果の種類(コードが意味を知っているのはここだけ。数値はエディタで決める)
enum class SkillEffectType
{
	None,				// 効果なし(つなぎのノードなど)

	// 数値(開放済みノードの値を種類ごとに足し合わせる)
	MiningPower,		// 採掘力 +value
	SwingSpeed,			// 振る速さ +value (0.2 = 20%速く)
	SwingScale,			// ハンマーの大きさ +value (0.3 = 今までのScaleUp 1回分)

	// 機能(開放したら使えるようになる。valueは使わない)
	UnlockRangeMining,	// 範囲採掘モード
};

// 機能を解放する効果か(数値を使わない)
bool IsFeatureEffect(SkillEffectType type);
// ツールチップに出す効果の文 (例: "採掘力 +50")
std::string MakeSkillEffectText(SkillEffectType type, float value);

// 必要素材1つ分
struct SkillCostItem
{
	ItemID id = ItemID::MAX;
	uint32_t count = 1;
};

// スキルツリーのノード1つ分(SkillTree.json に保存する。iconTextureID は読み込み時に入れる)
struct SkillNodeData
{
	// ノード番号(エディタが振る。開放状況のセーブがこの番号を指すので、後から変えない)
	int32_t id = -1;

	// 表示名
	std::string name;
	// 説明(\n で改行)
	std::string description;

	// アイコンの絵(空なら枠だけ)
	std::string iconPath;
	int32_t iconTextureID = -1;

	// ツリー上の位置(ノードの中心。px)
	Vector2 position;

	// 前提のノード番号。全部開放済みになったら開放できる(空なら最初から開放できる)
	std::vector<int32_t> parents;

	// 効果
	SkillEffectType effectType = SkillEffectType::None;
	float effectValue = 0.0f;

	// 消費レベル
	int32_t levelCost = 0;
	// 必要素材
	std::vector<SkillCostItem> itemCosts;
};
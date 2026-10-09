#pragma once
#include <definition/definition.h>
#include <cstdint>

// Event::dataに入れる型の置き場

// BlockDestroyed : 壊れたブロック
struct BlockDestroyedData
{
	BlockID id = BlockID::Air;	// 壊れたブロック
	Vector3 position;			// ブロック座標
};

// ItemPickup : 拾ったアイテム
struct ItemPickupData
{
	ItemID id = ItemID::MAX;	// 拾ったアイテム
	uint32_t count = 0;			// 個数
};

// ToolUpgradeRequested : ツールのグレードアップ依頼
enum class UpgradeType
{
	None,
	SpeedUp,
	ScaleUp,
	PowerUp,
};
struct ToolUpgradeRequest
{
	// 元の道具が入っているスロット
	int32_t slotIndex = -1;
	// 元の道具
	ItemID sourceID = ItemID::MAX;
	// グレードアップ先
	ItemID resultID = ItemID::MAX;
	// グレードアップの種類
	UpgradeType upgradeType = UpgradeType::None;
	// 消費レベル
	int32_t levelCost = 0;
};
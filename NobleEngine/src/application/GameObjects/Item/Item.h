#pragma once
#include "Game.h"

class Item
{
public:
	void Initialize(ItemID id);
	ItemID GetID() const { return id_; }
	int32_t GetTextureHandleForInventory() const { return textureHandleForInventory_; }
	int32_t GetTextureHandleForModel() const { return textureHandleForModel_; }
	int32_t GetModelHandle() const { return modelHandle_; }
	int32_t GetAbleStackCount() const { return ableStackCount_; } // 積載可能数（仮固定）

private:
	ItemID id_ = ItemID::None;

	// インベントリに表示する時のテクスチャ
	int32_t textureHandleForInventory_ = -1;
	// モデルとして描画する時のテクスチャ
	int32_t textureHandleForModel_ = -1;
	// モデルデータ
	int32_t modelHandle_ = -1;
	// 積載可能数
	int32_t ableStackCount_ = 64;

};

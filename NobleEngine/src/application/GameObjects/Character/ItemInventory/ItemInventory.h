#pragma once
#include <definition/definition.h>
#include <Game.h>
#include <array>

struct InventorySlot
{
    ItemID itemID = ItemID::MAX;
    uint8_t count = 0;
	int32_t iconTextureID = -1;
};

// IconのDrawはUIManagerの領分。ItemInventoryはアイテムの所持情報だけを管理する
class ItemInventory
{
public:
    ItemInventory();
    ~ItemInventory();
	void Initialize();

	// 今持っているアイテムのIDを取得
    ItemID GetCurrentSelectedItemID() const;

    // 所持アイテムに追加
    void AddItem(ItemID id);

private:

	// インベントリ情報(9x4)
    std::array<InventorySlot, 36> inventory_;
	// 装備欄情報(8x1)
    std::array<InventorySlot, 8> equipArea_;
    // クリックして持ち上げてるやつ
    InventorySlot hand_;

	// ホットバー選択インデックス
	int32_t hotbarSelectedIndex_ = 0;
};
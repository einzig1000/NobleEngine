#pragma once
#include <definition/definition.h>
#include <Game.h>
#include <array>

struct InventorySlot
{
    ItemID itemID = ItemID::MAX;
    uint32_t count = 0;
};

// IconのDrawはUIManagerの領分。ItemInventoryはアイテムの所持情報だけを管理する
class ItemInventory
{
public:

    static constexpr int32_t kSlotCount = 36;
    static constexpr int32_t kHotbarSlotCount = 9;
    static constexpr uint32_t kMaxStackCount = 999;
	static constexpr uint32_t kMaxEquipCount = 8;

    ItemInventory();
    ~ItemInventory();
	void Initialize();


	// 指定インデックスのアイテムを取得
    const InventorySlot& GetSlot(int32_t index) const { return inventory_[static_cast<size_t>(index)]; }
	// ホットバーの選択中スロットのインデックスを取得
    int32_t GetHotbarSelectedIndex() const { return hotbarSelectedIndex_; }
    // 今持っているアイテムのIDを取得
    ItemID  GetCurrentSelectedItemID() const;


    // 所持アイテムに追加
    void AddItem(ItemID id);
	// hpっとバーの選択中スロットを変更
    void SetHotbarSelectedIndex(int32_t index);
    // スロットごとアイテム入れ替え
    void SwapSlot(int32_t a, int32_t b);

private:

	// インベントリ情報(9x4)
    std::array<InventorySlot, kSlotCount> inventory_;
	// 装備欄情報(8x1)
    std::array<InventorySlot, kMaxEquipCount> equipArea_;
    // クリックして持ち上げてるやつ
    InventorySlot hand_;

	// ホットバー選択インデックス
	int32_t hotbarSelectedIndex_ = 0;
};
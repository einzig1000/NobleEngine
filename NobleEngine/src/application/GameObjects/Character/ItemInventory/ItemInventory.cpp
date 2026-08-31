#include "ItemInventory.h"

//int32_t y = index / 9;
//int32_t x = index % 9;

ItemInventory::ItemInventory()
{
}

ItemInventory::~ItemInventory() {}

void ItemInventory::Initialize()
{}

ItemID ItemInventory::GetCurrentSelectedItemID() const
{
	// ホットバーの選択中スロットを取得
	return inventory_[static_cast<size_t>(hotbarSelectedIndex_)].itemID;
}

// アイテム獲得（インベントリに追加）
void ItemInventory::AddItem(ItemID id)
{
    // 既存スタックに追加
    for (int32_t index = 0; index < kSlotCount; ++index)
    {
        auto& slot = inventory_[index];
        if (slot.itemID == id && slot.count < kMaxStackCount)
        {
            slot.count++;
            return;
        }
    }
    
    // 空スロットに追加
    for (int32_t index = 0; index < kSlotCount; ++index)
    {
        auto& slot = inventory_[index];
        if (slot.itemID == ItemID::MAX || slot.count == 0)
        {
            slot.itemID = id;
            slot.count = 1;
            return;
        }
    }
}

void ItemInventory::SetHotbarSelectedIndex(int32_t index)
{
    if (index < 0 || index >= kHotbarSlotCount) return;
    hotbarSelectedIndex_ = index;
}

void ItemInventory::SwapSlot(int32_t a, int32_t b)
{}

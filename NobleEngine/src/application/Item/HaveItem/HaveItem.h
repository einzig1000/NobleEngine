// HaveItem.h
#pragma once
#include "definition/definition.h"
#include <array>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "Item/Item.h"
#include "Game.h"

enum class SlotArea
{
    None,
    // インベントリ
    Inventory,
	// クラフト欄
    Craft,
	// クラフト結果欄
	Result,
	// 装備欄
    Head,
	Body,
    Leg,
    Boots,
};

struct SlotRef
{
    SlotArea area = SlotArea::None;
    int x = -1;
    int y = -1;

    bool IsValid() const { return area != SlotArea::None; }
};

struct InventorySlot
{
    Item item;
    uint8_t count = 0;
    std::array<std::unique_ptr<RenderData_Sprite>, 2> counter;
    std::unique_ptr<RenderData_Sprite> icon;

    bool IsEmpty() const { return item.GetID() == ItemID::None || count == 0; }
    void Clear()
    {
        item.Initialize(ItemID::None);
        count = 0;
        if (icon) icon->texture = -1;
    }
};

struct Hand
{
    Item item;
    uint8_t count = 0;
    std::array<std::unique_ptr<RenderData_Sprite>, 2> counter;
    std::unique_ptr<RenderData_Sprite> icon;

    bool IsEmpty() const { return item.GetID() == ItemID::None || count == 0; }
    void Clear()
    {
        item.Initialize(ItemID::None);
        count = 0;
        if (icon) icon->texture = -1;
    }
};

class MapManager;

class HaveItem
{
public:
    HaveItem();
    ~HaveItem();
	void SetMapManager(MapManager* mapManager) { mapManager_ = mapManager; }

    ItemID GetCurrentSelectedItemID() const;
    void RemoveCurrentSelectedItem(int count);

    void AddItem(ItemID id);
	// 持っているアイテムをドロップアイテムとして出現させる
	void DropCurrentSelectedItem(const Vector3& position);

    void UpdateInventory();
    void UpdateHotbar();

    void DrawInventory();
    void DrawHotbar();

	bool craftMode3x3_ = false;

private:
    // インベントリ情報
    std::array<std::array<InventorySlot, 9>, 4> inventory_;
    std::array<std::array<Vector3, 9>, 4> baseInventoryPositions_;

    // クラフト欄情報（2x2）
    std::array<std::array<InventorySlot, 2>, 2> craftArea_;
    std::array<std::array<Vector3, 2>, 2> baseCraftPositions_;

    // クラフト欄情報（3x3）
    std::array<std::array<InventorySlot, 3>, 3> craftArea3x3_;
    std::array<std::array<Vector3, 3>, 3> baseCraftPositions3x3_;

	// クラフト結果欄
    InventorySlot craftResultSlot_;
	Vector3 craftResultPosition_;

    // 装備欄情報（4スロット）
    std::array<InventorySlot, 4> equipArea_;
    std::array<Vector3, 4> baseEquipPositions_;

    // ホットバー
    std::array<InventorySlot, 9> hotbar_;

    // 手
    Hand hand_;

    // ホバー中のスロット
    SlotRef hoverSlot_;

    // ダブルクリック用
    int  lastClickFrame_ = 0;
    bool isDoubleClickPending_ = false;

    // ドラッグ分配
    bool isDragging_ = false;
    std::vector<SlotRef> dragSlots_;
    std::vector<std::pair<SlotRef, int>> dragPreview_;

	// ホットバー選択インデックス
	RenderData_Sprite hotbarSelector_;
	int hotbarSelectedIndex_ = 0;

    // 入力処理
    void UpdateHoverIndex();
    void UpdateLeftClick();
    void UpdateRightClick();
    void UpdateDrag();
	void UpdateHotbarInput();

    // 共通処理
    void UpdateCounters();
    void SyncHotbar();

    // 汎用スロットアクセス
    InventorySlot* GetSlot(const SlotRef& ref);
    Vector3        GetSlotBasePos(const SlotRef& ref);

	// クラフト
    std::array<std::array<ItemID, 3>, 3> GetCraftMatrix();
    void CheckCraftRecipe();
    void SetCraftResult(ItemID id, int count);

	MapManager* mapManager_ = nullptr;
};
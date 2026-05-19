// HaveItem.cpp
#include "Item/HaveItem/HaveItem.h"
#include "Item/CraftRecipe/CraftRecipe.h"
#include "MapManager/MapManager.h"

static Rect GetCraftRect(const std::array<std::array<ItemID, 3>, 3>& craft)
{
    Rect r;
    r.minX = r.minY = 3;
    r.maxX = r.maxY = -1;
    r.empty = true;

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            if (craft[y][x] != ItemID::None)
            {
                r.empty = false;
                r.minX = my_min(r.minX, x);
                r.minY = my_min(r.minY, y);
                r.maxX = my_max(r.maxX, x);
                r.maxY = my_max(r.maxY, y);
            }
        }
    }
    return r;
}

static bool MatchPatternRect(
    const std::array<std::array<ItemID, 3>, 3>& craft,
    const std::array<std::array<ItemID, 3>, 3>& pattern,
    const Vector2int& pMin,
    const Vector2int& pMax,
    const Rect& cRect)
{
    int pw = pMax.x - pMin.x + 1;
    int ph = pMax.y - pMin.y + 1;
    int cw = cRect.maxX - cRect.minX + 1;
    int ch = cRect.maxY - cRect.minY + 1;

    if (pw != cw || ph != ch)
        return false;

    for (int dy = 0; dy < ph; ++dy)
    {
        for (int dx = 0; dx < pw; ++dx)
        {
            ItemID p = pattern[pMin.y + dy][pMin.x + dx];
            ItemID c = craft[cRect.minY + dy][cRect.minX + dx];

            if (p != c)
                return false;
        }
    }

    return true;
}



HaveItem::HaveItem()
{
    // インベントリ基準位置
    float yOffset[4];
    yOffset[0] = 0.0f;
    yOffset[1] = yOffset[0] - 89.0f;
    yOffset[2] = yOffset[1] - 72.0f;
    yOffset[3] = yOffset[2] - 72.0f;

    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 9; ++x)
        {
            baseInventoryPositions_[y][x] = Vector3{
                static_cast<float>(352.0f + x * 72),
                static_cast<float>(593.0f + yOffset[y]),
                0.0f };

            auto& slot = inventory_[y][x];
            slot.item.Initialize(ItemID::None);
            slot.count = 0;

            slot.icon = std::make_unique<RenderData_Sprite>();
            slot.icon->anchor = Anchor::Center;
            slot.icon->transforms.translate = baseInventoryPositions_[y][x];
            slot.icon->texture = -1;

            slot.counter[0] = std::make_unique<RenderData_Sprite>();
            slot.counter[0]->anchor = Anchor::Center;
            slot.counter[0]->transforms.translate = baseInventoryPositions_[y][x] + Vector3{ 16.0f, 16.0f, 0.0f };
            slot.counter[0]->texture = -1;

            slot.counter[1] = std::make_unique<RenderData_Sprite>();
            slot.counter[1]->anchor = Anchor::Center;
            slot.counter[1]->transforms.translate = baseInventoryPositions_[y][x] + Vector3{ 32.0f, 16.0f, 0.0f };
            slot.counter[1]->texture = -1;
        }
    }

    // クラフト欄基準位置
    for (int y = 0; y < 2; ++y)
    {
        for (int x = 0; x < 2; ++x)
        {
            baseCraftPositions_[y][x] = Vector3{
                static_cast<float>(712.0f + x * 72),
                static_cast<float>(205.0f - y * 72),
                0.0f };

            auto& slot = craftArea_[y][x];
            slot.item.Initialize(ItemID::None);
            slot.count = 0;

            slot.icon = std::make_unique<RenderData_Sprite>();
            slot.icon->anchor = Anchor::Center;
            slot.icon->transforms.translate = baseCraftPositions_[y][x];
            slot.icon->texture = -1;

            slot.counter[0] = std::make_unique<RenderData_Sprite>();
            slot.counter[0]->anchor = Anchor::Center;
            slot.counter[0]->transforms.translate = baseCraftPositions_[y][x] + Vector3{ 16.0f, 16.0f, 0.0f };
            slot.counter[0]->texture = -1;

            slot.counter[1] = std::make_unique<RenderData_Sprite>();
            slot.counter[1]->anchor = Anchor::Center;
            slot.counter[1]->transforms.translate = baseCraftPositions_[y][x] + Vector3{ 32.0f, 16.0f, 0.0f };
            slot.counter[1]->texture = -1;
        }
    }
    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            baseCraftPositions3x3_[y][x] = Vector3{
                static_cast<float>(640.0f + x * 72),
                static_cast<float>(248.0f - y * 72),
                0.0f };

            auto& slot = craftArea3x3_[y][x];
            slot.item.Initialize(ItemID::None);
            slot.count = 0;
            slot.icon = std::make_unique<RenderData_Sprite>();
            slot.icon->anchor = Anchor::Center;
            slot.icon->transforms.translate = baseCraftPositions3x3_[y][x];
            slot.icon->texture = -1;
            slot.counter[0] = std::make_unique<RenderData_Sprite>();
			slot.counter[0]->anchor = Anchor::Center;
			slot.counter[0]->transforms.translate = baseCraftPositions3x3_[y][x] + Vector3{ 16.0f, 16.0f, 0.0f };
			slot.counter[0]->texture = -1;
			slot.counter[1] = std::make_unique<RenderData_Sprite>();
			slot.counter[1]->anchor = Anchor::Center;
			slot.counter[1]->transforms.translate = baseCraftPositions3x3_[y][x] + Vector3{ 32.0f, 16.0f, 0.0f };
            slot.counter[1]->texture = -1;
        }
	}

	// クラフト結果欄位置
    craftResultPosition_ = Vector3{ 928.0f, 173.0f, 0.0f };
    craftResultSlot_.item.Initialize(ItemID::None);
    craftResultSlot_.count = 0;

    craftResultSlot_.icon = std::make_unique<RenderData_Sprite>();
    craftResultSlot_.icon->anchor = Anchor::Center;
    craftResultSlot_.icon->transforms.translate = craftResultPosition_;
    craftResultSlot_.icon->texture = -1;

    craftResultSlot_.counter[0] = std::make_unique<RenderData_Sprite>();
    craftResultSlot_.counter[0]->anchor = Anchor::Center;
    craftResultSlot_.counter[0]->transforms.translate = craftResultPosition_ + Vector3{ 16.0f, 16.0f, 0.0f };
    craftResultSlot_.counter[0]->texture = -1;

    craftResultSlot_.counter[1] = std::make_unique<RenderData_Sprite>();
    craftResultSlot_.counter[1]->anchor = Anchor::Center;
    craftResultSlot_.counter[1]->transforms.translate = craftResultPosition_ + Vector3{ 32.0f, 16.0f, 0.0f };
    craftResultSlot_.counter[1]->texture = -1;

    // 装備欄基準位置
	baseEquipPositions_[0] = Vector3{ 424.0f,104.0f,0.0f }; // 頭
	baseEquipPositions_[1] = Vector3{ 568.0f,104.0f,0.0f }; // 胴
	baseEquipPositions_[2] = Vector3{ 424.0f,248.0f,0.0f }; // 脚
	baseEquipPositions_[3] = Vector3{ 568.0f,248.0f,0.0f }; // 足
    for (int i = 0; i < 4; ++i)
    {

        auto& slot = equipArea_[i];
        slot.item.Initialize(ItemID::None);
        slot.count = 0;

        slot.icon = std::make_unique<RenderData_Sprite>();
        slot.icon->anchor = Anchor::Center;
        slot.icon->transforms.translate = baseEquipPositions_[i];
        slot.icon->texture = -1;

        slot.counter[0] = std::make_unique<RenderData_Sprite>();
        slot.counter[0]->anchor = Anchor::Center;
        slot.counter[0]->transforms.translate = baseEquipPositions_[i] + Vector3{ 16.0f, 16.0f, 0.0f };
        slot.counter[0]->texture = -1;

        slot.counter[1] = std::make_unique<RenderData_Sprite>();
        slot.counter[1]->anchor = Anchor::Center;
        slot.counter[1]->transforms.translate = baseEquipPositions_[i] + Vector3{ 32.0f, 16.0f, 0.0f };
        slot.counter[1]->texture = -1;
    }

    // ホットバー
    for (int x = 0; x < 9; ++x)
    {
        auto& slot = hotbar_[x];
        slot.item.Initialize(ItemID::None);
        slot.count = 0;

        slot.icon = std::make_unique<RenderData_Sprite>();
        slot.icon->anchor = Anchor::Center;
        slot.icon->transforms.translate = Vector3{ 320.0f + x * 80.0f, 670.0f, 0.0f };
        slot.icon->texture = -1;

        slot.counter[0] = std::make_unique<RenderData_Sprite>();
        slot.counter[0]->anchor = Anchor::Center;
        slot.counter[0]->transforms.translate = Vector3{ 320.0f + x * 80.0f + 16.0f, 670.0f + 16.0f, 0.0f };
        slot.counter[0]->texture = -1;

        slot.counter[1] = std::make_unique<RenderData_Sprite>();
        slot.counter[1]->anchor = Anchor::Center;
        slot.counter[1]->transforms.translate = Vector3{ 320.0f + x * 80.0f + 32.0f, 670.0f + 16.0f, 0.0f };
        slot.counter[1]->texture = -1;
    }

    // 手
    hand_.item.Initialize(ItemID::None);
    hand_.count = 0;
    hand_.icon = std::make_unique<RenderData_Sprite>();
    hand_.icon->anchor = Anchor::Center;
    hand_.icon->texture = -1;

    hand_.counter[0] = std::make_unique<RenderData_Sprite>();
    hand_.counter[0]->anchor = Anchor::Center;
    hand_.counter[0]->texture = -1;

    hand_.counter[1] = std::make_unique<RenderData_Sprite>();
    hand_.counter[1]->anchor = Anchor::Center;
    hand_.counter[1]->texture = -1;


    // ホットバー選択枠
    hotbarSelector_.texture = ResourceID::GetUITextureID(UITextureID::Hotbar_Selected);
    hotbarSelector_.anchor = Anchor::Center;
    hotbarSelector_.transforms.scale = Vector3(1.0f, 1.0f, 1.0f);
    hotbarSelector_.transforms.translate = Vector3(640.0f, 670.0f, 0.0f);
}

HaveItem::~HaveItem()
{}

ItemID HaveItem::GetCurrentSelectedItemID() const
{
	// ホットバーの選択中スロットを取得
	return hotbar_[hotbarSelectedIndex_].item.GetID();
}

void HaveItem::RemoveCurrentSelectedItem(int count)
{
	// ホットバーの選択中スロットを取得 hotbar_[hotbarSelectedIndex_] = inventory_
	auto& slot = inventory_[0][hotbarSelectedIndex_];
    // アイテム数を減らす
    if (slot.count >= count)
    {
        slot.count -= count;
    }
    else
    {
        slot.count = 0;
    }
    // 空になったらクリア
    if (slot.count == 0)
    {
        slot.Clear();
	}
}

// アイテム獲得（インベントリに追加）
void HaveItem::AddItem(ItemID id)
{
    // 既存スタックに追加
    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 9; ++x)
        {
            auto& slot = inventory_[y][x];
            if (slot.item.GetID() == id && slot.count < slot.item.GetAbleStackCount())
            {
                slot.count++;
                return;
            }
        }
    }

    // 空スロットに追加
    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 9; ++x)
        {
            auto& slot = inventory_[y][x];
            if (slot.item.GetID() == ItemID::None || slot.count == 0)
            {
                slot.item.Initialize(id);
                slot.count = 1;
                slot.icon->texture = ResourceID::Get2DTextureID(id);
                return;
            }
        }
    }
}

void HaveItem::DropCurrentSelectedItem(const Vector3& position)
{
	// ハンドのアイテムをドロップ
    if (!hand_.IsEmpty())
    {
        for (int i = 0; i < hand_.count; ++i)
        {
            mapManager_->AddDropItemAt(position, hand_.item.GetID());
		}
        hand_.Clear();
        return;
	}
}

void HaveItem::UpdateInventory()
{
    UpdateHoverIndex();
    UpdateLeftClick();   // 左クリック（通常・Shift・ダブルクリック）
    UpdateRightClick();  // 右クリック（半分持つ & 1個置く）
    UpdateDrag();        // 左ドラッグ分配（インベントリ）

    // 手のアイコンをマウスに追従
    Vector2 mousePos = Game::IO::Mouse::GetPosition();
    if (!hand_.IsEmpty())
    {
        hand_.icon->transforms.translate = Vector3(mousePos.x, mousePos.y, 0);

        if (hand_.count >= 10)
        {
            int tens = hand_.count / 10;
            int units = hand_.count % 10;

            hand_.counter[0]->texture = ResourceID::GetNumberTextureID(tens);
            hand_.counter[1]->texture = ResourceID::GetNumberTextureID(units);

            hand_.counter[0]->transforms.translate = Vector3(mousePos.x + 16, mousePos.y + 16, 0);
            hand_.counter[1]->transforms.translate = Vector3(mousePos.x + 32, mousePos.y + 16, 0);
        }
        else
        {
            hand_.counter[0]->texture = ResourceID::GetNumberTextureID(hand_.count);
            hand_.counter[1]->texture = -1;

            hand_.counter[0]->transforms.translate = Vector3(mousePos.x + 24, mousePos.y + 16, 0);
        }
    }
    else
    {
        hand_.counter[0]->texture = -1;
        hand_.counter[1]->texture = -1;
    }

    UpdateCounters();
    SyncHotbar();
}

void HaveItem::UpdateHotbar()
{
	UpdateHotbarInput();
    UpdateCounters();
    SyncHotbar();
}



void HaveItem::DrawInventory()
{
    // インベントリ
    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 9; ++x)
        {
            auto& slot = inventory_[y][x];
            slot.icon->transforms.translate = baseInventoryPositions_[y][x];
            slot.icon->Draw();
            slot.counter[0]->Draw();
            slot.counter[1]->Draw();
        }
    }

    // クラフト欄
    if (craftMode3x3_)
    {
        for (int y = 0; y < 3; ++y)
        {
            for (int x = 0; x < 3; ++x)
            {
                auto& slot = craftArea3x3_[y][x];
                slot.icon->transforms.translate = baseCraftPositions3x3_[y][x];
                slot.icon->Draw();
                slot.counter[0]->Draw();
                slot.counter[1]->Draw();
            }
		}

    }
    else
    {
        for (int y = 0; y < 2; ++y)
        {
            for (int x = 0; x < 2; ++x)
            {
                auto& slot = craftArea_[y][x];
                slot.icon->transforms.translate = baseCraftPositions_[y][x];
                slot.icon->Draw();
                slot.counter[0]->Draw();
                slot.counter[1]->Draw();
            }
        }
    }

    // クラフト結果欄
    {
		auto& slot = craftResultSlot_;
        slot.icon->transforms.translate = craftResultPosition_;
        slot.icon->Draw();
        slot.counter[0]->Draw();
        slot.counter[1]->Draw();
	}

    // 装備欄
    for (int i = 0; i < 4; ++i)
    {
        auto& slot = equipArea_[i];
        slot.icon->transforms.translate = baseEquipPositions_[i];
        slot.icon->Draw();
        slot.counter[0]->Draw();
        slot.counter[1]->Draw();
    }

    // 手
    if (!hand_.IsEmpty())
    {
        hand_.icon->Draw();
        hand_.counter[0]->Draw();
        hand_.counter[1]->Draw();
    }

    // ドラッグ中プレビュー
    if (isDragging_)
    {
        for (auto& p : dragPreview_)
        {
            const SlotRef& ref = p.first;
            int previewCount = p.second;

            InventorySlot* slot = GetSlot(ref);
            if (!slot) continue;

            Vector3 basePos = GetSlotBasePos(ref);

            // アイコン描画
            slot->icon->transforms.translate = basePos;
            slot->icon->Draw();

            // カウンター描画
            if (previewCount >= 10)
            {
                int tens = previewCount / 10;
                int units = previewCount % 10;

                slot->counter[0]->texture = ResourceID::GetNumberTextureID(tens);
                slot->counter[1]->texture = ResourceID::GetNumberTextureID(units);

                slot->counter[0]->transforms.translate = basePos + Vector3{ 16,16,0 };
                slot->counter[1]->transforms.translate = basePos + Vector3{ 32,16,0 };
            }
            else
            {
                slot->counter[0]->texture = ResourceID::GetNumberTextureID(previewCount);
                slot->counter[1]->texture = -1;

                slot->counter[0]->transforms.translate = basePos + Vector3{ 24,16,0 };
            }

            slot->counter[0]->Draw();
            slot->counter[1]->Draw();
        }
    }
}

void HaveItem::DrawHotbar()
{
	// ホットバー
    for (int x = 0; x < 9; ++x)
    {
        auto& slot = hotbar_[x];
        slot.icon->Draw();
        slot.counter[0]->Draw();
        slot.counter[1]->Draw();
    }
	// 選択枠描画
	hotbarSelector_.Draw();
}


void HaveItem::UpdateHoverIndex()
{
    Vector2 mousePos = Game::IO::Mouse::GetPosition();
    hoverSlot_ = { SlotArea::None, -1, -1 };

    // クラフト欄
    if (craftMode3x3_)
    {
        for (int y = 0; y < 3; ++y)
        {
            for (int x = 0; x < 3; ++x)
            {
                Vector3 pos = baseCraftPositions3x3_[y][x];
                if (mousePos.x >= pos.x - 32 && mousePos.x <= pos.x + 32 &&
                    mousePos.y >= pos.y - 32 && mousePos.y <= pos.y + 32)
                {
                    hoverSlot_ = { SlotArea::Craft, x, y };
                    return;
                }
            }
        }
	}
    else
    {
        for (int y = 0; y < 2; ++y)
        {
            for (int x = 0; x < 2; ++x)
            {
                Vector3 pos = baseCraftPositions_[y][x];
                if (mousePos.x >= pos.x - 32 && mousePos.x <= pos.x + 32 &&
                    mousePos.y >= pos.y - 32 && mousePos.y <= pos.y + 32)
                {
                    hoverSlot_ = { SlotArea::Craft, x, y };
                    return;
                }
            }
        }
    }

    // インベントリ
    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 9; ++x)
        {
            Vector3 pos = baseInventoryPositions_[y][x];
            if (mousePos.x >= pos.x - 32 && mousePos.x <= pos.x + 32 &&
                mousePos.y >= pos.y - 32 && mousePos.y <= pos.y + 32)
            {
                hoverSlot_ = { SlotArea::Inventory, x, y };
            }
        }
    }

    // クラフト結果欄
    {
        Vector3 pos = craftResultPosition_;
        if (mousePos.x >= pos.x - 32 && mousePos.x <= pos.x + 32 &&
            mousePos.y >= pos.y - 32 && mousePos.y <= pos.y + 32)
        {
            hoverSlot_ = { SlotArea::Result, 0, 0 };
        }
    }

    // 装備
    Vector3 pos = baseEquipPositions_[0];
    if (mousePos.x >= pos.x - 32 && mousePos.x <= pos.x + 32 &&
        mousePos.y >= pos.y - 32 && mousePos.y <= pos.y + 32)
    {
		hoverSlot_ = { SlotArea::Head, 0, 0 };
    }
	pos = baseEquipPositions_[1];
    if (mousePos.x >= pos.x - 32 && mousePos.x <= pos.x + 32 &&
        mousePos.y >= pos.y - 32 && mousePos.y <= pos.y + 32)
    {
        hoverSlot_ = { SlotArea::Body, 0, 0 };
	}
    pos = baseEquipPositions_[2];
    if (mousePos.x >= pos.x - 32 && mousePos.x <= pos.x + 32 &&
        mousePos.y >= pos.y - 32 && mousePos.y <= pos.y + 32)
    {
        hoverSlot_ = { SlotArea::Leg, 0, 0 };
	}
    pos = baseEquipPositions_[3];
    if (mousePos.x >= pos.x - 32 && mousePos.x <= pos.x + 32 &&
        mousePos.y >= pos.y - 32 && mousePos.y <= pos.y + 32)
    {
        hoverSlot_ = { SlotArea::Boots, 0, 0 };
	}

}

void HaveItem::UpdateLeftClick()
{
    // ダブルクリック判定
    if (Game::IO::Mouse::IsJustPressed(0))
    {
        int currentFrame = Game::Time::GetElapsedTime();
        if (!isDragging_)
        {
            if (currentFrame - lastClickFrame_ <= 10)
                isDoubleClickPending_ = true;
        }
        lastClickFrame_ = currentFrame;
    }


    // 離した瞬間に処理
    if (!Game::IO::Mouse::IsJustReleased(0))
        return;

    // ドラッグ中は処理しない
    if (isDragging_)
        return;

    // 現在ホバー中のスロットを取得
    InventorySlot* slot = GetSlot(hoverSlot_);

    // スロットが無効なら何もしない
    if (!slot)
    {
        isDoubleClickPending_ = false;
        return;
    }

    // クラフト結果欄クリック → 素材消費
    if (hoverSlot_.area == SlotArea::Result)
    {
        bool shift = Game::IO::Key::IsHeld(DIK_LSHIFT) || Game::IO::Key::IsHeld(DIK_RSHIFT);

        // 結果が空なら何もしない
        if (craftResultSlot_.IsEmpty())
        {
            isDoubleClickPending_ = false;
            return;
        }

        if (shift)
        {
            // Shiftクリック → 作れるだけインベントリに送る
            while (true)
            {
                // 素材消費（2×2 or 3×3）
                if (craftMode3x3_)
                {
                    for (int y = 0; y < 3; ++y)
                    {
                        for (int x = 0; x < 3; ++x)
                        {
                            if (!craftArea3x3_[y][x].IsEmpty())
                            {
                                craftArea3x3_[y][x].count--;
                                if (craftArea3x3_[y][x].count == 0)
                                    craftArea3x3_[y][x].Clear();
                            }
                        }
                    }
                }
                else
                {
                    for (int y = 0; y < 2; ++y)
                    {
                        for (int x = 0; x < 2; ++x)
                        {
                            if (!craftArea_[y][x].IsEmpty())
                            {
                                craftArea_[y][x].count--;
                                if (craftArea_[y][x].count == 0)
                                    craftArea_[y][x].Clear();
                            }
                        }
                    }
                }


                // 結果をインベントリに送る（AddItem を resultCount 回）
                for (int i = 0; i < craftResultSlot_.count; ++i)
                {
                    AddItem(craftResultSlot_.item.GetID());
                }

                // 再判定
                CheckCraftRecipe();
                if (craftResultSlot_.IsEmpty())
                    break;
            }

            return;
        }
        else
        {
            // 通常クリック（今の処理）: 手に持つ & 素材を1回分消費
            if (hand_.IsEmpty())
            {
                hand_.item = craftResultSlot_.item;
                hand_.count = craftResultSlot_.count;
                hand_.icon->texture = craftResultSlot_.icon->texture;

                if (craftMode3x3_)
                {
                    for (int y = 0; y < 3; ++y)
                    {
                        for (int x = 0; x < 3; ++x)
                        {
                            if (!craftArea3x3_[y][x].IsEmpty())
                            {
                                craftArea3x3_[y][x].count--;
                                if (craftArea3x3_[y][x].count == 0)
                                    craftArea3x3_[y][x].Clear();
                            }
                        }
                    }
                }
                else
                {
                    for (int y = 0; y < 2; ++y)
                    {
                        for (int x = 0; x < 2; ++x)
                        {
                            if (!craftArea_[y][x].IsEmpty())
                            {
                                craftArea_[y][x].count--;
                                if (craftArea_[y][x].count == 0)
                                    craftArea_[y][x].Clear();
                            }
                        }
                    }
                }

                CheckCraftRecipe();
            }
            return;
        }

    }

    // ダブルクリック処理
    if (isDoubleClickPending_)
    {
        isDoubleClickPending_ = false;

        ItemID target = ItemID::None;
        int able = 0;

		// 手にアイテムがある場合
        if (!hand_.IsEmpty())
        {
            target = hand_.item.GetID();
            able = hand_.item.GetAbleStackCount();
        }
		// ダブルクリックしたスロットにアイテムがある場合
        else if (!slot->IsEmpty())
        {
            target = slot->item.GetID();
            able = slot->item.GetAbleStackCount();

            hand_.item = slot->item;
            hand_.count = slot->count;
            hand_.icon->texture = slot->icon->texture;
            slot->Clear();
        }
		// アイテムが無い場合は通常クリック扱い
        else
        {
            goto NORMAL_LEFT_CLICK;
        }


        // 全エリアから集める
        auto collectFrom2D = [&](auto& area)
            {
                for (auto& row : area)
                {
                    for (auto& s : row)
                    {
                        if (s.item.GetID() == target)
                        {
                            int canTake = able - hand_.count;
                            if (canTake <= 0) return;

                            int take = std::min<int>(s.count, canTake);
                            hand_.count += take;
                            s.count -= take;

                            if (s.count == 0)
                            {
                                s.item.Initialize(ItemID::None);
                                s.icon->texture = -1;
                            }
                        }
                    }
                }
            };


        collectFrom2D(inventory_);
        if (craftMode3x3_)collectFrom2D(craftArea3x3_);
        else collectFrom2D(craftArea_);



        for (int i = 0; i < 4; ++i)
        {
            auto& s = equipArea_[i];
            if (s.item.GetID() == target)
            {
                int canTake = able - hand_.count;
                if (canTake <= 0) break;

                int take = std::min<int>(s.count, canTake);
                hand_.count += take;
                s.count -= take;

                if (s.count == 0)
                {
                    s.item.Initialize(ItemID::None);
                    s.icon->texture = -1;
                }
            }
        }

        return;
    }
    isDoubleClickPending_ = false;

NORMAL_LEFT_CLICK:

    // Shiftクリック（インベントリのみ）
    bool shift = Game::IO::Key::IsHeld(DIK_LSHIFT) || Game::IO::Key::IsHeld(DIK_RSHIFT);

    // シフトクリックして移動させる場合の空きチェック
    SlotRef emptyCheckRef;
    bool foundEmpty = false;// 空きスロット見つけたか

    emptyCheckRef.area = SlotArea::Inventory;
    emptyCheckRef.y = (hoverSlot_.y != 0) ? 0 : 3;
    emptyCheckRef.x = 0;

    if (emptyCheckRef.y == 3)
    {
        for (int x = 0; x < 9; ++x)
        {
            if (inventory_[3][x].IsEmpty())
            {
                emptyCheckRef.x = x;
                foundEmpty = true;
                break;
            }
        }
        if (!foundEmpty)
        {
            emptyCheckRef.y = 2;
            for (int x = 0; x < 9; ++x)
            {
                if (inventory_[2][x].IsEmpty())
                {
                    emptyCheckRef.x = x;
                    foundEmpty = true;
                    break;
                }
            }
            if (!foundEmpty)
            {
                emptyCheckRef.y = 1;
                for (int x = 0; x < 9; ++x)
                {
                    if (inventory_[1][x].IsEmpty())
                    {
                        emptyCheckRef.x = x;
                        foundEmpty = true;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        for (int x = 0; x < 9; ++x)
        {
            if (inventory_[0][x].IsEmpty())
            {
                emptyCheckRef.x = x;
                foundEmpty = true;
                break;
            }
        }
    }


	// Shiftクリック（[装備品の場合]インベントリ→装備欄）　（[通常アイテムの場合]インベントリ→インベントリ）
    if (hoverSlot_.area == SlotArea::Inventory && shift)
    {
        // マウスのスロットが空なら何もしない
        if (slot->IsEmpty()) return;

        // 防具だったら装備欄に優先的に置く(今はなにももっていない)
		if (GetItemGenre(slot->item.GetID()) == ItemGenre::Head)
        {
            auto& dst = equipArea_[0];
            if (dst.IsEmpty())
            {
                dst.item = slot->item;
                dst.count = slot->count;
                dst.icon->texture = slot->icon->texture;
                slot->Clear();
                return;
            }
        }
        else if (GetItemGenre(slot->item.GetID()) == ItemGenre::Body)
        {
            auto& dst = equipArea_[1];
            if (dst.IsEmpty())
            {
                dst.item = slot->item;
                dst.count = slot->count;
                dst.icon->texture = slot->icon->texture;
                slot->Clear();
                return;
            }
        }
        else if (GetItemGenre(slot->item.GetID()) == ItemGenre::Leg)
        {
            auto& dst = equipArea_[2];
            if (dst.IsEmpty())
            {
                dst.item = slot->item;
                dst.count = slot->count;
                dst.icon->texture = slot->icon->texture;
                slot->Clear();
                return;
            }
        }
        else if (GetItemGenre(slot->item.GetID()) == ItemGenre::Boots)
        {
            auto& dst = equipArea_[3];
            if (dst.IsEmpty())
            {
                dst.item = slot->item;
                dst.count = slot->count;
                dst.icon->texture = slot->icon->texture;
                slot->Clear();
                return;
            }
        }

		// [0]から既存スタックに詰める場合は[3]から探索
		// [1~3]は[0]から探索
		// なければ空スロットに移動
        for (int x = 0; x < 9; ++x)
        {
            for (int y = 0; y < 4; ++y)
            {
                auto& dst = inventory_[y][x];
                if (dst.item.GetID() == slot->item.GetID() &&
                    dst.count < dst.item.GetAbleStackCount())
                {
                    int canTake = dst.item.GetAbleStackCount() - dst.count;
                    int take = std::min<int>(slot->count, canTake);
                    dst.count += take;
                    slot->count -= take;
                    if (slot->count == 0)
                    {
                        slot->Clear();
                        return;
                    }
                }
            }
        }

        // 空スロットに移動
        if (foundEmpty)
        {
            auto& dst = inventory_[emptyCheckRef.y][emptyCheckRef.x];
            dst.item = slot->item;
            dst.count = slot->count;
            dst.icon->texture = slot->icon->texture;
            slot->Clear();
        }

        return;
    }

	// Shiftクリック（装備欄→インベントリ）
    else if ((hoverSlot_.area == SlotArea::Head || hoverSlot_.area == SlotArea::Body || hoverSlot_.area == SlotArea::Leg || hoverSlot_.area == SlotArea::Boots)
        && shift)
    {
        if (slot->IsEmpty()) return;

        // 空スロットに移動
        if (foundEmpty)
        {
            auto& dst = inventory_[emptyCheckRef.y][emptyCheckRef.x];
            dst.item = slot->item;
            dst.count = slot->count;
            dst.icon->texture = slot->icon->texture;
            slot->Clear();
        }
    }

    // 装備欄には防具以外置けない
    else if (hoverSlot_.area == SlotArea::Head)
    {
        // 手に持っているアイテムが防具でなければ置けない
        if (!hand_.IsEmpty())
        {
            if (GetItemGenre(hand_.item.GetID()) != ItemGenre::Head)return;
        }
    }
    else if (hoverSlot_.area == SlotArea::Body)
    {
        // 手に持っているアイテムが防具でなければ置けない
        if (!hand_.IsEmpty())
        {
            if (GetItemGenre(hand_.item.GetID()) != ItemGenre::Body) return;
        }
    }
    else if (hoverSlot_.area == SlotArea::Leg)
    {
        // 手に持っているアイテムが防具でなければ置けない
        if (!hand_.IsEmpty())
        {
            if (GetItemGenre(hand_.item.GetID()) != ItemGenre::Leg)return;
        }
    }
    else if (hoverSlot_.area == SlotArea::Boots)
    {
        // 手に持っているアイテムが防具でなければ置けない
        if (!hand_.IsEmpty())
        {
            if (GetItemGenre(hand_.item.GetID()) != ItemGenre::Boots) return;
        }
    }

    // 通常左クリック（全エリア共通）
    if (hand_.IsEmpty())
    {
		// 手に何も持っていない → スロットから持つ
        if (!slot->IsEmpty())
        {
            hand_.item = slot->item;
            hand_.count = slot->count;
            hand_.icon->texture = slot->icon->texture;
            slot->Clear();
        }
    }
    else
    {
		// 手に何か持っている → スロットに置く
		if (slot->IsEmpty())
        {
            slot->item = hand_.item;
            slot->count = hand_.count;
            slot->icon->texture = hand_.icon->texture;
            hand_.Clear();
        }
        else if (slot->item.GetID() == hand_.item.GetID())
        {
            int able = slot->item.GetAbleStackCount();
            int total = slot->count + hand_.count;

            if (total <= able)
            {
                slot->count = total;
                hand_.Clear();
            }
            else
            {
                slot->count = able;
                hand_.count = total - able;
            }
        }
        else
        {
            std::swap(slot->item, hand_.item);
            std::swap(slot->count, hand_.count);
            std::swap(slot->icon->texture, hand_.icon->texture);
        }
    }


	// クラフト結果再判定
    CheckCraftRecipe();
}

void HaveItem::UpdateRightClick()
{
    if (!Game::IO::Mouse::IsJustPressed(1))
        return;

    InventorySlot* slot = GetSlot(hoverSlot_);
    if (!slot) return;

    // 手が空 → 半分持つ
    if (hand_.IsEmpty())
    {
        if (slot->IsEmpty()) return;

        int half = slot->count / 2;
        if (half == 0) half = 1;

        hand_.item = slot->item;
        hand_.count = half;
        hand_.icon->texture = slot->icon->texture;

        slot->count -= half;
        if (slot->count == 0)
            slot->Clear();
    }
    else
    {
        // 手に何か持っている → 1個置く
        if (slot->IsEmpty())
        {
            slot->item = hand_.item;
            slot->count = 1;
            slot->icon->texture = hand_.icon->texture;

            hand_.count--;
            if (hand_.count == 0)
                hand_.Clear();
        }
        else if (slot->item.GetID() == hand_.item.GetID())
        {
            if (slot->count < slot->item.GetAbleStackCount())
            {
                slot->count++;
                hand_.count--;
                if (hand_.count == 0)
                    hand_.Clear();
            }
        }
    }

	// クラフト結果再判定
	CheckCraftRecipe();
}

void HaveItem::UpdateDrag()
{
    bool leftDown = Game::IO::Mouse::IsHeld(0);
    bool leftJustPressed = Game::IO::Mouse::IsJustPressed(0);
    bool leftJustReleased = Game::IO::Mouse::IsJustReleased(0);

    // ドラッグ開始（Inventory or Craft）
    if (leftJustPressed && !hand_.IsEmpty() &&
        (hoverSlot_.area == SlotArea::Inventory || hoverSlot_.area == SlotArea::Craft))
    {
        isDragging_ = true;
        dragSlots_.clear();
        dragPreview_.clear();

        dragSlots_.push_back(hoverSlot_);
    }

    // ドラッグ中
    if (isDragging_ && leftDown)
    {
        if (hoverSlot_.area == SlotArea::Inventory || hoverSlot_.area == SlotArea::Craft)
        {
            // 新規スロットなら追加
            auto it = std::find_if(dragSlots_.begin(), dragSlots_.end(),
                [&](const SlotRef& r)
                {
                    return r.area == hoverSlot_.area &&
                        r.x == hoverSlot_.x &&
                        r.y == hoverSlot_.y;
                });

            if (it == dragSlots_.end())
                dragSlots_.push_back(hoverSlot_);

            // プレビュー計算
            dragPreview_.clear();
            int total = int(dragSlots_.size());
            int perSlot = my_max(1, hand_.count / total);

            for (auto& ref : dragSlots_)
            {
                dragPreview_.push_back({ ref, perSlot });
            }
        }
    }

    // ドラッグ終了
    if (isDragging_ && leftJustReleased)
    {
        isDragging_ = false;

        int total = int(dragSlots_.size());
        int perSlot = my_max(1, hand_.count / total);

        for (auto& ref : dragSlots_)
        {
            InventorySlot* slot = GetSlot(ref);
            if (!slot) continue;

            if (slot->IsEmpty())
            {
                slot->item = hand_.item;
                slot->count = perSlot;
                slot->icon->texture = hand_.icon->texture;
            }
            else if (slot->item.GetID() == hand_.item.GetID())
            {
                int able = slot->item.GetAbleStackCount();
                int canPut = able - slot->count;
                int put = my_min(canPut, perSlot);
                slot->count += put;
            }

            hand_.count -= perSlot;
            if (hand_.count <= 0)
            {
                hand_.Clear();
                break;
            }
        }

        dragSlots_.clear();
        dragPreview_.clear();

        CheckCraftRecipe();
    }
}

void HaveItem::UpdateHotbarInput()
{
    int mouseWheel = Game::IO::Mouse::GetWheel();
    if (mouseWheel > 0)
    {
        hotbarSelectedIndex_--;
        if (hotbarSelectedIndex_ < 0)
        {
            hotbarSelectedIndex_ = 8;
        }
    }
    else if (mouseWheel < 0)
    {
        hotbarSelectedIndex_++;
        if (hotbarSelectedIndex_ > 8)
        {
            hotbarSelectedIndex_ = 0;
        }
    }

    if (Game::IO::Key::IsJustPressed(DIK_1))
    {
        hotbarSelectedIndex_ = 0;
    }
    else if (Game::IO::Key::IsJustPressed(DIK_2))
    {
        hotbarSelectedIndex_ = 1;
    }
    else if (Game::IO::Key::IsJustPressed(DIK_3))
    {
        hotbarSelectedIndex_ = 2;
    }
    else if (Game::IO::Key::IsJustPressed(DIK_4))
    {
        hotbarSelectedIndex_ = 3;
    }
    else if (Game::IO::Key::IsJustPressed(DIK_5))
    {
        hotbarSelectedIndex_ = 4;
    }
    else if (Game::IO::Key::IsJustPressed(DIK_6))
    {
        hotbarSelectedIndex_ = 5;
    }
    else if (Game::IO::Key::IsJustPressed(DIK_7))
    {
        hotbarSelectedIndex_ = 6;
    }
    else if (Game::IO::Key::IsJustPressed(DIK_8))
    {
        hotbarSelectedIndex_ = 7;
    }
    else if (Game::IO::Key::IsJustPressed(DIK_9))
    {
        hotbarSelectedIndex_ = 8;
    }

    // 選択枠位置更新
    hotbarSelector_.transforms.translate.x = 320.0f + hotbarSelectedIndex_ * 80.0f;
}

void HaveItem::UpdateCounters()
{
    auto updateSlot = [&](InventorySlot& slot, const Vector3& basePos)
        {
            int count = slot.count;

            if (count >= 10)
            {
                int tens = count / 10;
                int units = count % 10;

                slot.counter[0]->texture = ResourceID::GetNumberTextureID(tens);
                slot.counter[1]->texture = ResourceID::GetNumberTextureID(units);

                slot.counter[0]->transforms.translate = basePos + Vector3{ 16, 16, 0 };
                slot.counter[1]->transforms.translate = basePos + Vector3{ 32, 16, 0 };
            }
            else if (count > 0)
            {
                slot.counter[0]->texture = ResourceID::GetNumberTextureID(count);
                slot.counter[1]->texture = -1;

                slot.counter[0]->transforms.translate = basePos + Vector3{ 24, 16, 0 };
            }
            else
            {
                slot.counter[0]->texture = -1;
                slot.counter[1]->texture = -1;
            }
        };

    // インベントリ
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 9; ++x)
            updateSlot(inventory_[y][x], baseInventoryPositions_[y][x]);

    // クラフト欄
    if (craftMode3x3_)
    {
        for (int y = 0; y < 3; ++y)
            for (int x = 0; x < 3; ++x)
                updateSlot(craftArea3x3_[y][x], baseCraftPositions3x3_[y][x]);
    }
    else
    {
        for (int y = 0; y < 2; ++y)
            for (int x = 0; x < 2; ++x)
                updateSlot(craftArea_[y][x], baseCraftPositions_[y][x]);
    }

    // クラフト結果欄
    updateSlot(craftResultSlot_, craftResultPosition_);

    // 装備欄
    for (int i = 0; i < 4; ++i)
        updateSlot(equipArea_[i], baseEquipPositions_[i]);

    // ホットバー
    for (int x = 0; x < 9; ++x)
        updateSlot(hotbar_[x], hotbar_[x].icon->transforms.translate);
}


void HaveItem::SyncHotbar()
{
    for (int x = 0; x < 9; ++x)
    {
        auto& src = inventory_[0][x];
        auto& dst = hotbar_[x];

        dst.item = src.item;
        dst.count = src.count;
        dst.icon->texture = src.icon->texture;

        dst.counter[0]->texture = src.counter[0]->texture;
        dst.counter[1]->texture = src.counter[1]->texture;
    }
}

InventorySlot* HaveItem::GetSlot(const SlotRef& ref)
{
    switch (ref.area)
    {
    case SlotArea::Inventory:
        if (ref.y >= 0 && ref.y < 4 && ref.x >= 0 && ref.x < 9)
            return &inventory_[ref.y][ref.x];
        break;
    case SlotArea::Craft:
        if (craftMode3x3_)
        {
            if (ref.y >= 0 && ref.y < 3 && ref.x >= 0 && ref.x < 3)
                return &craftArea3x3_[ref.y][ref.x];
        }
        else
        {
            if (ref.y >= 0 && ref.y < 2 && ref.x >= 0 && ref.x < 2)
                return &craftArea_[ref.y][ref.x];
		}
        break;
    case SlotArea::Head:
        if (ref.area == SlotArea::Head)
            return &equipArea_[0];
        break;
    case SlotArea::Body:
        if (ref.area == SlotArea::Body)
            return &equipArea_[1];
        break;
    case SlotArea::Leg:
        if (ref.area == SlotArea::Leg)
            return &equipArea_[2];
        break;
    case SlotArea::Boots:
        if (ref.area == SlotArea::Boots)
            return &equipArea_[3];
        break;
    case SlotArea::Result:
        return &craftResultSlot_;
    default:
        break;
    }
    return nullptr;
}

Vector3 HaveItem::GetSlotBasePos(const SlotRef& ref)
{
    switch (ref.area)
    {
    case SlotArea::Inventory:
        return baseInventoryPositions_[ref.y][ref.x];
    case SlotArea::Craft:
        if (craftMode3x3_)
        {
            return baseCraftPositions3x3_[ref.y][ref.x];
        }
        else
        {
            return baseCraftPositions_[ref.y][ref.x];
		}
	case SlotArea::Head:
		return baseEquipPositions_[0];
	case SlotArea::Body:
		return baseEquipPositions_[1];
	case SlotArea::Leg:
		return baseEquipPositions_[2];
	case SlotArea::Boots:
		return baseEquipPositions_[3];
	case SlotArea::Result:
		return craftResultPosition_;
    default:
        return Vector3{ 0,0,0 };
    }
}

void HaveItem::SetCraftResult(ItemID id, int count)
{
    if (id == ItemID::None || count <= 0)
    {
        craftResultSlot_.Clear();
        return;
    }

    craftResultSlot_.item.Initialize(id);
    craftResultSlot_.count = count;
    craftResultSlot_.icon->texture = ResourceID::Get2DTextureID(id);
}

std::array<std::array<ItemID, 3>, 3> HaveItem::GetCraftMatrix()
{
    std::array<std::array<ItemID, 3>, 3> mat{};

    if (craftMode3x3_)
    {
        // 3x3モード
        for (int y = 0; y < 3; ++y)
        {
            for (int x = 0; x < 3; ++x)
            {
                mat[2 - y][x] = craftArea3x3_[y][x].item.GetID();
            }
        }
    }
    else
    {
        for (int y = 0; y < 2; ++y)
        {
            for (int x = 0; x < 2; ++x)
            {
                mat[1 - y][x] = craftArea_[y][x].item.GetID();
            }
        }
    }

    return mat;
}

// pattern の最小矩形を求める
void GetBoundingBox(
    const std::array<std::array<ItemID, 3>, 3>& pattern,
    int& minY, int& maxY, int& minX, int& maxX)
{
    minY = 3; maxY = -1;
    minX = 3; maxX = -1;

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            if (pattern[y][x] != ItemID::None)
            {
                minY = my_min(minY, y);
                maxY = my_max(maxY, y);
                minX = my_min(minX, x);
                maxX = my_max(maxX, x);
            }
        }
    }
}

bool MatchPatternAt(
    const std::array<std::array<ItemID, 3>, 3>& craft,
    const std::array<std::array<ItemID, 3>, 3>& pattern,
    const Vector2int& min,
    const Vector2int& max,
    int offsetY,
    int offsetX)
{
    for (int y = min.y; y <= max.y; ++y)
    {
        for (int x = min.x; x <= max.x; ++x)
        {
            int cy = y + offsetY;
            int cx = x + offsetX;

            if (cy < 0 || cy >= 3 || cx < 0 || cx >= 3)
                return false;

            if (craft[cy][cx] != pattern[y][x])
                return false;
        }
    }

    return true;
}



void HaveItem::CheckCraftRecipe()
{
    auto craft = GetCraftMatrix();
    Rect cRect = GetCraftRect(craft);

    if (cRect.empty)
    {
        SetCraftResult(ItemID::None, 0);
        return;
    }

    for (auto& recipe : CraftRecipeList::GetRecipeList())
    {
        for (size_t i = 0; i < recipe.pattern.size(); ++i)
        {
            const auto& pattern = recipe.pattern[i];
            const auto& pMin = recipe.mins[i];
            const auto& pMax = recipe.maxs[i];

            if (MatchPatternRect(craft, pattern, pMin, pMax, cRect))
            {
                SetCraftResult(recipe.resultID, recipe.resultCount);
                return;
            }
        }
    }

    SetCraftResult(ItemID::None, 0);
}

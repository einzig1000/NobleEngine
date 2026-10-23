#include "Inventory.h"
#include <GameObjects/Character/ItemInventory/ItemInventory.h>
#include <GameObjects/Character/MiningPointGauge/MiningPointGauge.h>
#include <GameObjects/UI/ItemIcon/ItemIconManager.h>
#include <System/EventBus/EventBus.h>
#include <App.h>
#include <algorithm>
#include <cmath>

namespace
{
	// スロットの中身が道具ならその情報を返す。道具以外・空ならnullptr
	const ItemInfo* GetToolInfo(const InventorySlot& slot)
	{
		if (slot.itemID == ItemID::MAX) return nullptr;
		const ItemInfo* info = App::Data::Item::Get(slot.itemID);
		if (!info || info->genre != ItemGenre::Tool) return nullptr;
		return info;
	}

}

Inventory::Inventory()
{
	BuildLayout();

	// 動的スプライト
	inventoryIcons_.resize(static_cast<size_t>(ItemInventory::kSlotCount));
	for (ElementInstanceData& icon : inventoryIcons_)
	{
		icon.render = std::make_unique<RenderObject>();
		icon.render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
		icon.render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
		icon.render->psoConfig_.depthStencilID = DepthStencilID::Disable;
		icon.render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
		icon.render->SetupFromShaders();
	}

	accessoryIcons_.resize(static_cast<size_t>(ItemInventory::kMaxAccessoryCount));
	for (ElementInstanceData& icon : accessoryIcons_)
	{
		icon.render = std::make_unique<RenderObject>();
		icon.render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
		icon.render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
		icon.render->psoConfig_.depthStencilID = DepthStencilID::Disable;
		icon.render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
		icon.render->SetupFromShaders();
	}

	countTextOffset_ = { 0.0f, 0.0f };
}


Inventory::~Inventory()
{}

void Inventory::Initialize()
{
	selectedSlot_ = -1;
}

void Inventory::Update(int32_t cameraID)
{
	orthographic_ = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);
	
	// 採掘ptゲージとレベル
	if (miningGauge_)
	{
		// 塗りの幅を割合に合わせる
		if (ElementInstanceData* gauge = GetPart(Part::GaugeFill))
		{
			const float ratio = std::clamp(miningGauge_->GetGaugeRatio(), 0.0f, 1.0f);
			gauge->data.scale = Vector2{ gaugeFullScale_.x * ratio, gaugeFullScale_.y };
			gauge->color = gaugeFillColor_;
		}

		// レベルの文字
		levelText_ = std::to_string(miningGauge_->GetLevel());
		if (ElementInstanceData* marker = GetPart(Part::LevelText))
		{
			levelTextPos_ = GetDigitTextPosition(levelText_, levelCharSize_, GetCenter(marker->data));
		}
	}

	// 持ち物のアイコン
	if (inventory_)
	{
		// 動的スプライトの更新

		for (int32_t i = 0; i < ItemInventory::kSlotCount; ++i)
		{
			ElementInstanceData* slotSprite = GetInventorySlotSprite(i);

			inventoryIcons_[i].data.textureID = -1;
			if (!slotSprite || !iconManager_ || inventory_->GetInventorySlot(i).itemID == ItemID::MAX) continue;

			// アイコン(モデルから作ったアイコン)
			inventoryIcons_[i].data.textureID = iconManager_->GetIcon(inventory_->GetInventorySlot(i).itemID);

			// スロットの中心に置く(スロットのscale・回転に合わせる)
			const Vector2 iconSize = Vector2{ iconManager_->GetIconSize() * slotSprite->data.scale.x, iconManager_->GetIconSize() * slotSprite->data.scale.y };
			UpdateQuad(inventoryIcons_[i], GetCenter(slotSprite->data), iconSize, slotSprite->data.rotate, orthographic_);
		}
		for (int32_t i = 0; i < static_cast<int32_t>(ItemInventory::kMaxAccessoryCount); ++i)
		{
			ElementInstanceData* partSprite = GetPart(Part::AccessorySlots, i);

			accessoryIcons_[i].data.textureID = -1;
			if (!partSprite || !iconManager_ || inventory_->GetAccessorySlot(i).itemID == ItemID::MAX) continue;

			// アイコン(モデルから作ったアイコン)
			accessoryIcons_[i].data.textureID = iconManager_->GetIcon(inventory_->GetAccessorySlot(i).itemID);

			// スロットの中心に置く(スロットのscale・回転に合わせる)
			const Vector2 iconSize = Vector2{ iconManager_->GetIconSize() * partSprite->data.scale.x, iconManager_->GetIconSize() * partSprite->data.scale.y };
			UpdateQuad(accessoryIcons_[i], GetCenter(partSprite->data), iconSize, partSprite->data.rotate, orthographic_);
		}

		// クリック(スロットの選択)
		HandleClick();
	}

	// 静的スプライトの更新
	UpdateStaticRenderObject(orthographic_);
}

void Inventory::Draw(int32_t rt_ID)
{
	// 静的スプライトの描画
	DrawStaticRenderObject(rt_ID);

	if (inventory_)
	{
		// 動的スプライトの描画

		// アイコン
		for (const ElementInstanceData& icon : inventoryIcons_)
		{
			DrawRenderObject(rt_ID, icon);
		}
		for (const ElementInstanceData& icon : accessoryIcons_)
		{
			DrawRenderObject(rt_ID, icon);
		}

		// 個数
		for (int32_t i = 0; i < ItemInventory::kSlotCount; ++i)
		{
			const InventorySlot& item = inventory_->GetInventorySlot(i);
			const ElementInstanceData* slot = GetInventorySlotSprite(i);
			if (item.itemID == ItemID::MAX || !slot || item.count <= 1) continue;

			const Vector2 pos = Vector2{ GetCenter(slot->data).x + countTextOffset_.x, GetCenter(slot->data).y + countTextOffset_.y };
			Game::Asset::Font::DrawString(rt_ID, std::to_string(item.count), 46, pos, Vector4{ 0.0f, 0.0f, 0.0f, 1.0f });
		}
	}

	// レベル
	if (miningGauge_ && !levelText_.empty())
	{
		DrawOutlinedString(rt_ID, levelText_, levelCharSize_, levelTextPos_, gaugeFillColor_, 2.0f);
	}
}

void Inventory::DrawImGui()
{
	ImGui::Begin("Inventory");
	if (ImGui::Button("UIデータを読み直す"))
	{
		BuildLayout();
	}
	ImGui::Text("選んでいるスロット: %d", selectedSlot_);
	ImGui::DragFloat2("countTextOffset_", &countTextOffset_.x, 0.1f);
	ImGui::End();
}

void Inventory::BuildLayout()
{
	// sprites_にJsonのデータを入れてrenderObjectを生成する
	BuildSprites(UIElementType::Inventory);
	// Partで分類しlabelIndex順に並べたarrayを生成する
	parts_ = FindParts<Part>();

	// ゲージの塗りは、Jsonのscaleを満タンの大きさとして覚えておく
	gaugeFullScale_ = Vector2{ 0.0f, 0.0f };
	if (ElementInstanceData* gauge = GetPart(Part::GaugeFill))
	{
		gaugeFullScale_ = gauge->data.scale;
	}
}

ElementInstanceData* Inventory::GetPart(Part part, int32_t labelIndex)
{
	return GetSprite(parts_[static_cast<size_t>(part)], labelIndex);
}

ElementInstanceData* Inventory::GetInventorySlotSprite(int32_t index)
{
	// ホットバー段のスロットは、labelIndex 0〜8 = インベントリの0〜8番
	if (index < ItemInventory::kHotbarSlotCount)
	{
		return GetPart(Part::HotbarSlots, index);
	}
	// メイン3段のスロットは、labelIndex 0〜26 = インベントリの9〜35番
	return GetPart(Part::MainSlots, index - ItemInventory::kHotbarSlotCount);
}

void Inventory::HandleClick()
{
	if (!Game::IO::Mouse::IsJustPressed(0)) return;

	const Vector2 mouse = GetMouseUIPosition();

	// スロットをクリックしたら、道具なら選ぶ。道具以外・空なら選択を外す
	for (int32_t i = 0; i < ItemInventory::kSlotCount; ++i)
	{
		const ElementInstanceData* slot = GetInventorySlotSprite(i);
		if (!slot || !IsMouseOver(slot->data, mouse)) continue;

		selectedSlot_ = GetToolInfo(inventory_->GetInventorySlot(i)) ? i : -1;
		return;
	}
}

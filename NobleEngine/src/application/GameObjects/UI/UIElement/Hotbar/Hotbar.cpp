#include "Hotbar.h"
#include <GameObjects/Character/ItemInventory/ItemInventory.h>
#include <GameObjects/UI/ItemIcon/ItemIconManager.h>

Hotbar::Hotbar()
{
	BuildLayout();

	// 動的スプライト
	icons_.resize(static_cast<size_t>(ItemInventory::kHotbarSlotCount));
	for (ElementInstanceData& icon : icons_)
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

Hotbar::~Hotbar()
{}

void Hotbar::Initialize()
{}

void Hotbar::Update(int32_t cameraID)
{
	orthographic_ = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);

	// ホットバーのアイコン
	if (inventory_)
	{
		for (int32_t i = 0; i < ItemInventory::kHotbarSlotCount; ++i)
		{
			ElementInstanceData* partSprite = GetPart(Part::Slots, i);

			icons_[i].data.textureID = -1;
			if (!partSprite || !iconManager_ || inventory_->GetInventorySlot(i).itemID == ItemID::MAX) continue;

			// アイコン(モデルから作ったアイコン)
			icons_[i].data.textureID = iconManager_->GetIcon(inventory_->GetInventorySlot(i).itemID);

			// スロットの中心に置く(スロットのscale・回転に合わせる)
			const Vector2 iconSize = Vector2{ iconManager_->GetIconSize() * partSprite->data.scale.x, iconManager_->GetIconSize() * partSprite->data.scale.y };
			UpdateQuad(icons_[i], GetCenter(partSprite->data), iconSize, partSprite->data.rotate, orthographic_);
		}
	}

	// 静的スプライトの更新
	UpdateStaticRenderObject(orthographic_);
}

void Hotbar::Draw(int32_t rt_ID)
{
	// 静的スプライトの描画
	DrawStaticRenderObject(rt_ID);

	if (inventory_)
	{
		// アイコン
		for (const ElementInstanceData& icon : icons_)
		{
			if (icon.data.textureID < 0) continue;

			icon.render->SetBRegisterData(0, ShaderType::VertexShader, &icon.wvp);
			icon.render->SetBRegisterData(1, ShaderType::VertexShader, &icon.world);
			icon.render->SetBRegisterData(0, ShaderType::PixelShader, &icon.color);
			icon.render->SetBRegisterData(1, ShaderType::PixelShader, &icon.data.textureID);
			icon.render->Draw(rt_ID, { icon.data.textureID });
		}

		// 個数
		for (int32_t i = 0; i < ItemInventory::kHotbarSlotCount; ++i)
		{
			const InventorySlot& item = inventory_->GetInventorySlot(i);
			const ElementInstanceData* slot = GetPart(Part::Slots, i);
			if (item.itemID == ItemID::MAX || !slot || item.count <= 1) continue;

			const Vector2 pos = Vector2{ GetCenter(slot->data).x + countTextOffset_.x, GetCenter(slot->data).y + countTextOffset_.y };
			Game::Asset::Font::DrawString(rt_ID, std::to_string(item.count), 46, pos, Vector4{ 0.0f, 0.0f, 0.0f, 1.0f });
		}
	}
}

void Hotbar::DrawImGui()
{
	ImGui::Begin("Hotbar");
	if (ImGui::Button("UIデータを読み直す"))
	{
		BuildLayout();
	}
	ImGui::End();
}

void Hotbar::BuildLayout()
{
	BuildSprites(UIElementType::Hotbar);
	parts_ = FindParts<Part>();
}

ElementInstanceData* Hotbar::GetPart(Part part, int32_t labelIndex)
{
	return GetSprite(parts_[static_cast<size_t>(part)], labelIndex);
}

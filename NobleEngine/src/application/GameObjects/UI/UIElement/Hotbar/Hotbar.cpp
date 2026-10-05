#include "Hotbar.h"
#include <GameObjects/Character/ItemInventory/ItemInventory.h>
#include <GameObjects/UI/ItemIcon/ItemIconManager.h>
#include <App.h>

Hotbar::Hotbar()
{
	Vector2 windowSize = Vector2(float(Game::Window::GetWidth()), float(Game::Window::GetHeight()));
	 
	// sprites_[0] : ホットバー
	sprites_.emplace_back(ElementData{});
	sprites_[0].render = std::make_unique<RenderObject>();
	sprites_[0].render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	sprites_[0].render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprites_[0].render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	sprites_[0].render->SetupFromShaders();
	sprites_[0].textureID = Game::Asset::Texture::Load("assets/application/Minecraft/UI/Hotbar/Hotbar.png");
	const TextureData* textureData = Game::Asset::Texture::GetData(sprites_[0].textureID);
	sprites_[0].transforms.scale = Vector3(float(textureData->metadata.width) * 0.5f, float(textureData->metadata.height) * 0.5f, 1.0f);
	sprites_[0].transforms.translate = Vector3(windowSize.x * 0.5f, windowSize.y * 0.93f, 1.0f);

	// icons_ : スロットアイコン
	const int32_t planeModelID = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	icons_.reserve(static_cast<size_t>(ItemInventory::kHotbarSlotCount));
	for (int32_t i = 0; i < ItemInventory::kHotbarSlotCount; ++i)
	{
		ElementData icon{};
		icon.render = std::make_unique<RenderObject>();
		icon.render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
		icon.render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
		icon.render->modelID_ = planeModelID;
		icon.render->SetupFromShaders();

		icon.textureID = -1;
		icon.transforms.scale = Vector3(32.0f, 32.0f, 1.0f);
		icon.transforms.rotate = Vector3(0.0f, 0.0f, 0.0f);
		icon.transforms.translate = GetSlotPosition(i);

		icons_.emplace_back(std::move(icon));
		icons_[i].transforms.translate = GetSlotPosition(i);
	}
}

Hotbar::~Hotbar()
{}

void Hotbar::Initialize()
{}

void Hotbar::Update(int32_t cameraID)
{
	orthographic_ = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);

	if (!inventory_) return;

	// ホットバーのアイコン更新
	for (int32_t i = 0; i < ItemInventory::kHotbarSlotCount; ++i)
	{
		const InventorySlot& slot = inventory_->GetInventorySlot(i);
		// 空スロット
		if (slot.itemID == ItemID::MAX)
		{
			icons_[i].textureID = -1;
			continue;
		}

		// アイテム情報取得
		const ItemInfo* info = App::Data::Item::Get(slot.itemID);
		// 情報なし or テクスチャIDなし
		if (!info || info->textureID < 0)
		{
			icons_[i].textureID = -1;
			continue;
		}

		// アイコン更新
		icons_[i].textureID = iconManager_ ? iconManager_->GetIcon(slot.itemID) : -1;
	}
}

void Hotbar::Draw(int32_t rt_ID)
{
	for (const auto& sprite : sprites_)
	{
		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(sprite.transforms.scale, sprite.transforms.rotate, sprite.transforms.translate);
		Matrix4x4 wvp = world * orthographic_;
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

		sprite.render->SetBRegisterData(0, ShaderType::VertexShader, &wvp);
		sprite.render->SetBRegisterData(1, ShaderType::VertexShader, &world);
		sprite.render->SetBRegisterData(0, ShaderType::PixelShader, &color);
		sprite.render->SetBRegisterData(1, ShaderType::PixelShader, &sprite.textureID);

		sprite.render->Draw(rt_ID);
	}

	if (!inventory_) return;

	// アイコン
	for (int32_t i = 0; i < ItemInventory::kHotbarSlotCount; ++i)
	{
		if (icons_[i].textureID < 0) continue;

		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(icons_[i].transforms.scale, icons_[i].transforms.rotate, icons_[i].transforms.translate);
		Matrix4x4 wvp = world * orthographic_;
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

		icons_[i].render->SetBRegisterData(0, ShaderType::VertexShader, &wvp);
		icons_[i].render->SetBRegisterData(1, ShaderType::VertexShader, &world);
		icons_[i].render->SetBRegisterData(0, ShaderType::PixelShader, &color);
		icons_[i].render->SetBRegisterData(1, ShaderType::PixelShader, &icons_[i].textureID);
		icons_[i].render->Draw(rt_ID, { icons_[i].textureID });
	}

	// 個数
	for (int32_t i = 0; i < ItemInventory::kHotbarSlotCount; ++i)
	{
		if (icons_[i].textureID < 0) continue;

		const InventorySlot& slot = inventory_->GetInventorySlot(i);
		if (slot.count > 1)
		{
			const Vector3& iconPos = icons_[i].transforms.translate;
			Game::Asset::Font::DrawString(rt_ID, std::to_string(slot.count), 46, Vector2{ iconPos.x + 6.0f, iconPos.y + 6.0f }, Vector4{ 0.0f,0.0f,0.0f,1.0f });
		}
	}
}

void Hotbar::DrawImGui()
{}

Vector3 Hotbar::GetSlotPosition(int32_t index) const
{
	// ホットバーの左上の座標
	Vector3 hotbarTopLeft = sprites_[0].transforms.translate - Vector3(sprites_[0].transforms.scale.x, sprites_[0].transforms.scale.y, 0.0f);
	// ホットバーの1スロットの幅
	float slotWidth = sprites_[0].transforms.scale.x * 2.0f / static_cast<float>(ItemInventory::kHotbarSlotCount);
	// ホットバーの1スロットの高さ
	float slotHeight = sprites_[0].transforms.scale.y;
	// 指定されたスロットの位置
	return hotbarTopLeft + Vector3(slotWidth * index + slotWidth * 0.5f, slotHeight, 0.0f);
}

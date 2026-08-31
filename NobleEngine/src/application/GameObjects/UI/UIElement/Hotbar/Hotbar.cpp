#include "Hotbar.h"
#include <GameObjects/Character/ItemInventory/ItemInventory.h>
#include <externals/MagicEnum/magic_enum.hpp>
#include <App.h>

Hotbar::Hotbar()
{
	// sprites_[0] : ホットバー
	sprites_.emplace_back(ElementData{});
	sprites_[0].render = std::make_unique<RenderObject>();
	sprites_[0].render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	sprites_[0].render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprites_[0].render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	sprites_[0].render->SetupFromShaders();
	sprites_[0].textureID = Game::Asset::Texture::Load("assets/application/Minecraft/UI/Inventory/Hotbar.png");
	const TextureData* textureData = Game::Asset::Texture::GetData(sprites_[0].textureID);
	sprites_[0].transforms.scale = Vector3(float(textureData->metadata.width) / 2.0f, float(textureData->metadata.height) / 2.0f, 1.0f);
	sprites_[0].transforms.translate = Vector3(640.0f, 670.0f, 1.0f);

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
	}
}

Hotbar::~Hotbar()
{}

void Hotbar::Initialize()
{}

void Hotbar::Update(int32_t cameraID)
{
	Matrix4x4 orthographic = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);
	for (const auto& sprite : sprites_)
	{
		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(sprite.transforms.scale, sprite.transforms.rotate, sprite.transforms.translate);
		Matrix4x4 wvp = world * orthographic;
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

		sprite.render->SetCBufferData(0, ShaderType::VertexShader, &wvp);
		sprite.render->SetCBufferData(1, ShaderType::VertexShader, &world);
		sprite.render->SetCBufferData(0, ShaderType::PixelShader, &color);
		sprite.render->SetCBufferData(1, ShaderType::PixelShader, &sprite.textureID);
	}

	if (!inventory_) return;
	for (int32_t i = 0; i < ItemInventory::kHotbarSlotCount; ++i)
	{
		auto& icon = icons_[i];
		const InventorySlot& slot = inventory_->GetSlot(i);

		// 空スロット
		if (slot.itemID == ItemID::MAX)
		{
			icon.textureID = -1;
			continue;
		}

		const ItemInfo* info = App::Data::Item::Get(slot.itemID);
		//if (!info || info->iconID < 0)
		if (!info || info->textureID < 0)
		{
			icon.textureID = -1;
			continue;

			//std::string tag = std::string("itemIcon_") + std::string(magic_enum::enum_name(slot.itemID));
			//info->iconID = Game::Asset::RenderTexture::CreateRenderTexture(32, 32, tag);
		}

		//icon.textureID = info->iconID;
		icon.textureID = info->textureID;
		icon.transforms.translate = GetSlotPosition(i);

		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(icon.transforms.scale, icon.transforms.rotate, icon.transforms.translate);
		Matrix4x4 wvp = world * orthographic;
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

		icon.render->SetCBufferData(0, ShaderType::VertexShader, &wvp);
		icon.render->SetCBufferData(1, ShaderType::VertexShader, &world);
		icon.render->SetCBufferData(0, ShaderType::PixelShader, &color);
		icon.render->SetCBufferData(1, ShaderType::PixelShader, &icon.textureID);
	}
}

void Hotbar::Draw(int32_t rt_ID)
{
	for (const auto& sprite : sprites_)
	{
		sprite.render->Draw(rt_ID);
	}

	if (!inventory_) return;
	for (int32_t i = 0; i < ItemInventory::kHotbarSlotCount; ++i)
	{
		if (icons_[i].textureID < 0) continue;
		icons_[i].render->Draw(rt_ID);

		// 個数
		const InventorySlot& slot = inventory_->GetSlot(i);
		if (slot.count > 1)
		{
			const Vector3& p = icons_[i].transforms.translate;
			Game::Asset::Font::DrawString(rt_ID, std::to_string(slot.count), 32, Vector2{ p.x + 6.0f, p.y + 6.0f }, Vector4{ 1.0f,0.3f,0.3f,1.0f });
		}
	}
}

Vector3 Hotbar::GetSlotPosition(int32_t index) const
{
	// sprites_[0].transforms.scale
	// sprites_[0].transforms.translate

	// ホットバーの左上の座標
	Vector3 hotbarTopLeft = sprites_[0].transforms.translate - Vector3(sprites_[0].transforms.scale.x, sprites_[0].transforms.scale.y, 0.0f);
	// ホットバーの1スロットの幅
	float slotWidth = sprites_[0].transforms.scale.x * 2.0f / static_cast<float>(ItemInventory::kHotbarSlotCount);
	// ホットバーの1スロットの高さ
	float slotHeight = sprites_[0].transforms.scale.y;
	// 指定されたスロットの位置
	return hotbarTopLeft + Vector3(slotWidth * index + slotWidth * 0.5f, slotHeight, 0.0f);
}

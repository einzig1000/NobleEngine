#include "UIManager/ScreenMode/UIElement/Inventory/Inventory.h"

Inventory::Inventory()
{
	// sprites_[0] : インベントリ背景
	sprites_.emplace_back(std::make_unique<RenderData_Sprite>());
	sprites_[0]->texture = ResourceID::GetTextureID(TextureID::white1x1);
	sprites_[0]->transforms.scale = Vector3(2000.0f, 2000.0f, 1.0f);
	sprites_[0]->anchor = Anchor::LeftTop;
	sprites_[0]->transforms.translate = Vector3(-10.0f, -10.0f, 0.0f);
	sprites_[0]->color = 0x00000077;

	// sprites_[1] : アイテムインベントリ
	sprites_.emplace_back(std::make_unique<RenderData_Sprite>());
	sprites_[1]->texture = ResourceID::GetUITextureID(UITextureID::Inventory2x2);
	sprites_[1]->anchor = Anchor::Center;
	sprites_[1]->transforms.scale = Vector3(1.0f, 1.0f, 1.0f);
	sprites_[1]->transforms.translate = Vector3(640.0f, 325.0f, 0.0f);
}

Inventory::~Inventory()
{}

void Inventory::Initialize()
{}

void Inventory::Update()
{

}

void Inventory::Draw()
{
	for (const auto& sprite : sprites_)
	{
		sprite->Draw();
	}
}

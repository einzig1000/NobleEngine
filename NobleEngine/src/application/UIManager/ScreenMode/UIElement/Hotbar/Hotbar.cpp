#include "UIManager/ScreenMode/UIElement/Hotbar/Hotbar.h"

Hotbar::Hotbar()
{
	// sprites_[0] : ホットバー
	sprites_.emplace_back(std::make_unique<RenderData_Sprite>());
	sprites_[0]->texture = ResourceID::GetUITextureID(UITextureID::Hotbar);
	sprites_[0]->anchor = Anchor::Center;
	sprites_[0]->transforms.scale = Vector3(1.0f, 1.0f, 1.0f);
	sprites_[0]->transforms.translate = Vector3(640.0f, 670.0f, 0.0f);
}

Hotbar::~Hotbar()
{}

void Hotbar::Initialize()
{}

void Hotbar::Update()
{
}

void Hotbar::Draw()
{
	for (const auto& sprite : sprites_)
	{
		sprite->Draw();
	}
}

#include "UIManager/ScreenMode/UIElement/Pause/Pause.h"
#include "MapManager/MapManager.h"

Pause::Pause()
{
	// sprites_[0] : ポーズ画面背景
	sprites_.emplace_back(std::make_unique<RenderData_Sprite>());
	sprites_[0]->texture = ResourceID::GetTextureID(TextureID::white1x1);
	sprites_[0]->transforms.scale = Vector3(2000.0f, 2000.0f, 1.0f);
	sprites_[0]->anchor = Anchor::LeftTop;
	sprites_[0]->transforms.translate = Vector3(-10.0f, -10.0f, 0.0f);
	sprites_[0]->color = 0x00000077;

	// sprites_[1] : ポーズメニュー
	sprites_.emplace_back(std::make_unique<RenderData_Sprite>());
	sprites_[1]->texture = ResourceID::GetUITextureID(UITextureID::PAUSE_menu);
	sprites_[1]->anchor = Anchor::Center;
	sprites_[1]->transforms.scale = Vector3(1.0f, 1.0f, 1.0f);
	sprites_[1]->transforms.translate = Vector3(640.0f, 325.0f, 0.0f);

	// sprites_[2] : 保存ボタン
	sprites_.emplace_back(std::make_unique<RenderData_Sprite>());
	sprites_[2]->texture = ResourceID::GetUITextureID(UITextureID::PAUSE_SaveButton);
	sprites_[2]->anchor = Anchor::Center;
	sprites_[2]->transforms.scale = Vector3(1.0f, 1.0f, 1.0f);
	sprites_[2]->transforms.translate = Vector3(640.0f, 325.0f, 0.0f);
}

Pause::~Pause()
{}

void Pause::Initialize()
{}

void Pause::Update()
{
	if (sprites_[2]->isCollisionMouseRay)
	{
		sprites_[2]->color = 0xFFFFFFFF;
		if (Game::IO::Mouse::IsJustPressed(0))
		{
			saveRequested_ = true;	
		}
	}
	else
	{
		sprites_[2]->color = 0x777777FF;
	}
}

void Pause::Draw()
{
	for (const auto& sprite : sprites_)
	{
		sprite->Draw();
	}
	sprites_[2]->DrawImGui();
}

bool Pause::ConsumeSaveRequested()
{
	if (!saveRequested_) return false;
	saveRequested_ = false;
	return true;
}

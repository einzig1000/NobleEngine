#include "UIManager/ScreenMode/PlayingScreen/PlayingScreen.h"
#include "UIManager/ScreenMode/UIElement/Hotbar/Hotbar.h"
#include "Character/Player/Player.h"

PlayingScreen::PlayingScreen()
{
	// uiElements_[0] : Hotbar
	uiElements_.emplace_back(std::make_unique<Hotbar>());
}

PlayingScreen::~PlayingScreen()
{
}

void PlayingScreen::Initialize()
{
	nextUIMode_ = UIMode::None;

	for (const auto& element : uiElements_)
	{
		element->Initialize();
	}

	// カーソル操作無効化
	Game::IO::Mouse::ShowCursor(false);
}

void PlayingScreen::Update()
{
	for (const auto& element : uiElements_)
	{
		element->Update();
	}

	if (Game::IO::Key::IsJustPressed(DIK_E))
	{
		nextUIMode_ = UIMode::Inventory;
	}

	if (Game::IO::Key::IsJustPressed(DIK_ESCAPE))
	{
		nextUIMode_ = UIMode::Pause;
	}
}

void PlayingScreen::Draw()
{
	uiElements_[0]->Draw();		// Hotbar

	player_->DrawHotbar();		// Hotbarアイコン描画

}
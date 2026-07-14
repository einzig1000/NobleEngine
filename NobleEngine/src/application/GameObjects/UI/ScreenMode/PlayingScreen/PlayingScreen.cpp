#include "PlayingScreen.h"
#include <GameObjects/UI/UIElement/Hotbar/Hotbar.h>
#include <GameObjects/Character/Player/Player.h>

PlayingScreen::PlayingScreen()
{
	// uiElements_[0] : Hotbar
	uiElements_.emplace_back(std::make_unique<Hotbar>());

	renderTargetID_ = Game::Asset::RenderTexture::CreateRenderTexture(Game::Window::GetWidth(), Game::Window::GetHeight(), "PlayingUIScreen");
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
	//Game::IO::Mouse::ShowCursor(false);
}

void PlayingScreen::Update(int32_t cameraID)
{
	for (const auto& element : uiElements_)
	{
		element->Update(cameraID);
	}


	//if (Game::IO::Key::IsJustPressed('E'))
	//{
	//	nextUIMode_ = UIMode::Inventory;
	//}
	//if (Game::IO::Key::IsJustPressed(VK_ESCAPE))
	//{
	//	nextUIMode_ = UIMode::Pause;
	//}
}

void PlayingScreen::Draw()
{
	for (const auto& element : uiElements_)
	{
		element->Draw(renderTargetID_);
	}

	//uiElements_[0]->Draw(renderTargetID_);		// Hotbar
	//player_->DrawHotbar();		// Hotbarアイコン描画
}
#include "PlayingScreen.h"
#include <GameObjects/UI/UIElement/Hotbar/Hotbar.h>

PlayingScreen::PlayingScreen()
{
	elementTypes_.push_back(UIElementType::Hotbar);
}

PlayingScreen::~PlayingScreen()
{
}

void PlayingScreen::Initialize()
{
	nextUIMode_ = UIMode::MAX;

	for (const auto& element : uiElements_)
	{
		element->Initialize();
		element->SetNextUIMode(&nextUIMode_);
	}

	// カーソル操作有効化
	Game::IO::Mouse::ShowCursor(false);
}

void PlayingScreen::Update(int32_t cameraID)
{
	for (const auto& element : uiElements_)
	{
		element->Update(cameraID);
	}


	if (Game::IO::Key::IsJustPressed('E'))
	{
		nextUIMode_ = UIMode::Inventory;
	}
	if (Game::IO::Key::IsJustPressed('F'))
	{
		nextUIMode_ = UIMode::MiningMode;
	}
	if (Game::IO::Key::IsJustPressed(VK_ESCAPE))
	{
		nextUIMode_ = UIMode::Pause;
	}
}

void PlayingScreen::Draw(int32_t renderTargetID)
{
	for (const auto& element : uiElements_)
	{
		element->Draw(renderTargetID);
	}
}
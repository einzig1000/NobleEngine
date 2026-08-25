#include "CraftScreen.h"
#include <GameObjects/UI/UIElement/Craft/Craft.h>
#include <GameObjects/Character/Player/Player.h>

CraftScreen::CraftScreen()
{
	elementTypes_.push_back(UIElementType::Craft);
}

CraftScreen::~CraftScreen()
{}

void CraftScreen::Initialize()
{
	nextUIMode_ = UIMode::MAX;

	for (const auto& element : uiElements_)
	{
		element->Initialize();
		element->SetNextUIMode(&nextUIMode_);
	}

	// カーソル操作有効化
	Game::IO::Mouse::ShowCursor(true);
}

void CraftScreen::Update(int32_t cameraID)
{
	for (const auto& element : uiElements_)
	{
		element->Update(cameraID);
	}

	if (Game::IO::Key::IsJustPressed(VK_ESCAPE) ||
		Game::IO::Key::IsJustPressed('E'))
	{
		nextUIMode_ = UIMode::Playing;

		// カーソル操作無効化
		Game::IO::Mouse::ShowCursor(false);
	}
}

void CraftScreen::Draw(int32_t renderTargetID)
{
	for (const auto& element : uiElements_)
	{
		element->Draw(renderTargetID);
	}

	//player_->DrawInventory();	// Inventoryアイコン描画
}
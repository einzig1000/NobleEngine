#include "InventoryScreen.h"
#include <GameObjects/UI/UIElement/Inventory/Inventory.h>
#include <GameObjects/UI/UIElement/Hotbar/Hotbar.h>
#include <GameObjects/Character/Player/Player.h>

InventoryScreen::InventoryScreen()
{
	elementTypes_.push_back(UIElementType::Hotbar);
	elementTypes_.push_back(UIElementType::Inventory);
}

InventoryScreen::~InventoryScreen()
{}

void InventoryScreen::Initialize()
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

void InventoryScreen::Update(int32_t cameraID)
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

void InventoryScreen::Draw(int32_t renderTargetID)
{
	for (const auto& element : uiElements_)
	{
		element->Draw(renderTargetID);
	}
}

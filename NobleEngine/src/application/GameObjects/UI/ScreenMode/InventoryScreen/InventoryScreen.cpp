#include "InventoryScreen.h"
#include <GameObjects/UI/UIElement/Inventory/Inventory.h>
#include <GameObjects/UI/UIElement/Hotbar/Hotbar.h>
#include <GameObjects/Character/Player/Player.h>

InventoryScreen::InventoryScreen()
{
	// uiElements_[1] : Inventory
	uiElements_.emplace_back(std::make_unique<Inventory>());

	renderTargetID_ = Game::Asset::RenderTexture::CreateRenderTexture(Game::Window::GetWidth(), Game::Window::GetHeight(), "InventoryUIScreen");
}

InventoryScreen::~InventoryScreen()
{}

void InventoryScreen::Initialize()
{
	nextUIMode_ = UIMode::None;

	for (const auto& element : uiElements_)
	{
		element->Initialize();
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

void InventoryScreen::Draw()
{
	for (const auto& element : uiElements_)
	{
		element->Draw(renderTargetID_);
	}

	//player_->DrawInventory();	// Inventoryアイコン描画
}

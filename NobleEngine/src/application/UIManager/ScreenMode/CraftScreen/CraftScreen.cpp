#include "UIManager/ScreenMode/CraftScreen/CraftScreen.h"
#include "UIManager/ScreenMode/UIElement/Craft/Craft.h"
#include "UIManager/ScreenMode/UIElement/Hotbar/Hotbar.h"
#include "Character/Player/Player.h"

CraftScreen::CraftScreen()
{
	// uiElements_[0] : Hotbar
	uiElements_.emplace_back(std::make_unique<Hotbar>());
	// uiElements_[1] : Craft
	uiElements_.emplace_back(std::make_unique<Craft>());
}

CraftScreen::~CraftScreen()
{}

void CraftScreen::Initialize()
{
	nextUIMode_ = UIMode::None;
	for (const auto& element : uiElements_)
	{
		element->Initialize();
	}
	// カーソル操作有効化
	Game::IO::Mouse::ShowCursor(true);
}

void CraftScreen::Update()
{
	for (const auto& element : uiElements_)
	{
		element->Update();
	}

	if (Game::IO::Key::IsJustPressed(DIK_ESCAPE) ||
		Game::IO::Key::IsJustPressed(DIK_E))
	{
		nextUIMode_ = UIMode::Playing;
	}
}

void CraftScreen::Draw()
{
	uiElements_[0]->Draw();		// Hotbar

	player_->DrawHotbar();		// Hotbarアイコン描画

	uiElements_[1]->Draw();		// Inventory

	player_->DrawInventory();	// Inventoryアイコン描画
}
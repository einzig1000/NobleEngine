#include "UIManager/UIManager.h"
#include "UIManager/ScreenMode/PlayingScreen/PlayingScreen.h"
#include "UIManager/ScreenMode/InventoryScreen/InventoryScreen.h"
#include "UIManager/ScreenMode/CraftScreen/CraftScreen.h"
#include "UIManager/ScreenMode/PauseScreen/PauseScreen.h"


UIManager::UIManager()
{
	playingScreen_ = std::make_unique<PlayingScreen>();
	inventoryScreen_ = std::make_unique<InventoryScreen>();
	craftingScreen_ = std::make_unique<CraftScreen>();
	pauseScreen_ = std::make_unique<PauseScreen>();
}

UIManager::~UIManager(){}

void UIManager::Initialize()
{
	playingScreen_->SetPlayer(player_);
	playingScreen_->SetMapManager(mapManager_);

	inventoryScreen_->SetPlayer(player_);
	inventoryScreen_->SetMapManager(mapManager_);

	craftingScreen_->SetPlayer(player_);
	craftingScreen_->SetMapManager(mapManager_);

	pauseScreen_->SetPlayer(player_);
	pauseScreen_->SetMapManager(mapManager_);

	ChangeScreen(UIMode::Playing);
}

void UIManager::Update()
{
	if (currentScreen_)
	{
		currentScreen_->Update();
		// 画面遷移確認
		UIMode nextMode = currentScreen_->GetNextUIMode();
		if (nextMode != UIMode::None)
		{
			// 新画面生成
			ChangeScreen(nextMode);
		}
	}
}

void UIManager::Draw()
{
	if (currentScreen_)
	{
		currentScreen_->Draw();
	}
}

void UIManager::DrawImGui()
{}

void UIManager::ChangeScreen(UIMode mode)
{
	// 現在の画面破棄
	if (currentScreen_)
	{
		currentScreen_ = nullptr;
	}

	// 新しい画面生成
	switch (mode)
	{
	case UIMode::Playing:
		currentScreen_ = playingScreen_.get();
		break;
	case UIMode::Inventory:
		currentScreen_ = inventoryScreen_.get();
		break;
	case UIMode::Crafting:
		currentScreen_ = craftingScreen_.get();
		break;
	case UIMode::Pause:
		currentScreen_ = pauseScreen_.get();
		break;
	default:
		currentScreen_ = nullptr;
		break;
	}

	currentUIMode_ = mode;

	if (currentScreen_)
	{
		currentScreen_->Initialize();
	}
}

#include "UIManager.h"
#include "ScreenMode/UIScreen.h"


UIManager::UIManager()
{
	playingScreen_ = std::make_unique<PlayingScreen>();
	inventoryScreen_ = std::make_unique<InventoryScreen>();
	craftingScreen_ = std::make_unique<CraftScreen>();
	pauseScreen_ = std::make_unique<PauseScreen>();
}

void UIManager::SetPlayer(Player* player)
{
	playingScreen_->SetPlayer(player);
	inventoryScreen_->SetPlayer(player);
	craftingScreen_->SetPlayer(player);
	pauseScreen_->SetPlayer(player);
}

void UIManager::SetMapManager(MapManager * mapManager)
{
	playingScreen_->SetMapManager(mapManager);
	inventoryScreen_->SetMapManager(mapManager);
	craftingScreen_->SetMapManager(mapManager);
	pauseScreen_->SetMapManager(mapManager);
}

UIManager::~UIManager(){}

void UIManager::Initialize()
{
	ChangeScreen(UIMode::Playing);
}

void UIManager::Update(int32_t cameraID)
{
	if (currentScreen_)
	{
		currentScreen_->Update(cameraID);

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
	}

	currentUIMode_ = mode;

	if (currentScreen_)
	{
		currentScreen_->Initialize();
	}
}

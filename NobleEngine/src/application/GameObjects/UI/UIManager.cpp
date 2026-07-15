#include "UIManager.h"
#include "UIScreen/PlayingScreen/PlayingScreen.h"
#include "UIScreen/InventoryScreen/InventoryScreen.h"
#include "UIScreen/CraftScreen/CraftScreen.h"
#include "UIScreen/PauseScreen/PauseScreen.h"

#include "UIElement/Craft/Craft.h"
#include "UIElement/Hotbar/Hotbar.h"
#include "UIElement/Inventory/Inventory.h"
#include "UIElement/Pause/Pause.h"


UIManager::UIManager()
{
	elements_[UIElementType::Inventory] = std::make_unique<Inventory>();
	elements_[UIElementType::Craft] = std::make_unique<Craft>();
	elements_[UIElementType::Pause] = std::make_unique<Pause>();
	elements_[UIElementType::Hotbar] = std::make_unique<Hotbar>();

	screens_[UIMode::Playing] = std::make_unique<PlayingScreen>();
	screens_[UIMode::Inventory] = std::make_unique<InventoryScreen>();
	screens_[UIMode::Crafting] = std::make_unique<CraftScreen>();
	screens_[UIMode::Pause] = std::make_unique<PauseScreen>();

	for (auto& screen : screens_)
	{
		const auto& requiredElements = screen.second->GetRequiredElements();
		for (const auto& elementType : requiredElements)
		{
			if (elements_.find(elementType) != elements_.end())
			{
				screen.second->AddElement(elements_[elementType].get());
			}
		}
	}
}

void UIManager::SetPlayer(Player* player)
{
	for (auto& screen : screens_)
	{
		screen.second->SetPlayer(player);
	}
}

void UIManager::SetMapManager(MapManager * mapManager)
{
	for (auto& screen : screens_)
	{
		screen.second->SetMapManager(mapManager);
	}
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

void UIManager::Draw(int32_t renderTargetID)
{
	if (currentScreen_)
	{
		currentScreen_->Draw(renderTargetID);
	}
}

void UIManager::DrawImGui()
{}

void UIManager::ChangeScreen(UIMode mode)
{
	currentUIMode_ = mode;
	currentScreen_ = screens_[currentUIMode_].get();

	if (currentScreen_)
	{
		currentScreen_->Initialize();
	}
}

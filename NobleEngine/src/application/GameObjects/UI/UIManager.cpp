#include "UIManager.h"
#include "UIScreen/PlayingScreen/PlayingScreen.h"
#include "UIScreen/InventoryScreen/InventoryScreen.h"
#include "UIScreen/CraftScreen/CraftScreen.h"
#include "UIScreen/PauseScreen/PauseScreen.h"
#include "UIScreen/MiningModeScreen/MiningModeScreen.h"

#include "UIElement/Craft/Craft.h"
#include "UIElement/Hotbar/Hotbar.h"
#include "UIElement/Inventory/Inventory.h"
#include "UIElement/Pause/Pause.h"
#include "UIElement/MiningMode/MiningMode.h"


UIManager::UIManager()
{
	elements_[UIElementType::Inventory] = std::make_unique<Inventory>();
	elements_[UIElementType::Craft] = std::make_unique<Craft>();
	elements_[UIElementType::Pause] = std::make_unique<Pause>();
	elements_[UIElementType::Hotbar] = std::make_unique<Hotbar>();
	elements_[UIElementType::MiningMode] = std::make_unique<MiningMode>();

	screens_[UIMode::Playing] = std::make_unique<PlayingScreen>();
	screens_[UIMode::Inventory] = std::make_unique<InventoryScreen>();
	screens_[UIMode::Crafting] = std::make_unique<CraftScreen>();
	screens_[UIMode::Pause] = std::make_unique<PauseScreen>();
	screens_[UIMode::MiningMode] = std::make_unique<MiningModeScreen>();

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

bool UIManager::IsGameplayActive() const
{
	switch (currentUIMode_)
	{
	case UIMode::Playing:			return true;		break;
	case UIMode::Inventory:			return false;		break;
	case UIMode::Crafting:			return false;		break;
	case UIMode::Pause:				return false;		break;
	case UIMode::MiningMode:		return false;		break;
	}

	return true;
}

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
	elements_.resize(static_cast<size_t>(UIElementType::MAX));
	elements_[static_cast<size_t>(UIElementType::Inventory)] = std::make_unique<Inventory>();
	elements_[static_cast<size_t>(UIElementType::Craft)] = std::make_unique<Craft>();
	elements_[static_cast<size_t>(UIElementType::Pause)] = std::make_unique<Pause>();
	elements_[static_cast<size_t>(UIElementType::Hotbar)] = std::make_unique<Hotbar>();
	elements_[static_cast<size_t>(UIElementType::MiningMode)] = std::make_unique<MiningMode>();


	screens_.resize(static_cast<size_t>(UIMode::MAX));
	screens_[static_cast<size_t>(UIMode::Playing)] = std::make_unique<PlayingScreen>();
	screens_[static_cast<size_t>(UIMode::Inventory)] = std::make_unique<InventoryScreen>();
	screens_[static_cast<size_t>(UIMode::Crafting)] = std::make_unique<CraftScreen>();
	screens_[static_cast<size_t>(UIMode::Pause)] = std::make_unique<PauseScreen>();
	screens_[static_cast<size_t>(UIMode::MiningMode)] = std::make_unique<MiningModeScreen>();

	for (size_t i = 0; i < screens_.size(); ++i)
	{
		const auto& requiredElements = screens_[i]->GetRequiredElements();
		for (const auto& elementType : requiredElements)
		{
			screens_[i]->AddElement(elements_[static_cast<size_t>(elementType)].get());
		}
	}
}


void UIManager::SetEventBus(EventBus* eventBus)
{
	eventBus_ = eventBus;

	for (const auto& element : elements_)
	{
		element->SetEventBus(eventBus_);
	}

	for (const auto& screen : screens_)
	{
		screen->SetEventBus(eventBus_);
	}
}

void UIManager::SetInventory(const ItemInventory* inventory)
{
	inventory_ = inventory;

	for (auto& element : elements_)
	{
		element->SetInventory(inventory_);
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
		if (nextMode != UIMode::MAX)
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
	currentUIMode_ = static_cast<size_t>(mode);
	currentScreen_ = screens_[currentUIMode_].get();

	if (currentScreen_)
	{
		currentScreen_->Initialize();
	}
}
#include "InventoryScreen.h"
#include <GameObjects/UI/UIElement/Inventory/Inventory.h>
#include <System/EventBus/EventBus.h>

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

	if (eventBus_)
	{
		Event event;
		event.type = EventType::AbleMoveAllCharacters;
		event.value.push_back(false);
		eventBus_->Notify(event);
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
	}
}

void InventoryScreen::Draw(int32_t renderTargetID)
{
	for (const auto& element : uiElements_)
	{
		element->Draw(renderTargetID);
	}
}

#include "CraftScreen.h"
#include <GameObjects/UI/UIElement/Craft/Craft.h>
#include <System/EventBus/EventBus.h>

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

	if (eventBus_)
	{
		Event event;
		event.type = EventType::AbleMoveAllCharacters;
		event.data = false;
		eventBus_->Notify(event);
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

void CraftScreen::DrawImGui()
{
	for (const auto& element : uiElements_)
	{
		element->DrawImGui();
	}
}

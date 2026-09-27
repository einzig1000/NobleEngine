#include "EmptyScreen.h"
#include <GameObjects/UI/UIElement/IUIElement.h>
#include <System/EventBus/EventBus.h>

EmptyScreen::EmptyScreen()
{
}

EmptyScreen::~EmptyScreen()
{}

void EmptyScreen::Initialize()
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
		event.value.push_back(true);
		eventBus_->Notify(event);
	}

	// カーソル操作有効化
	Game::IO::Mouse::ShowCursor(false);
}

void EmptyScreen::Update(int32_t cameraID)
{
	for (const auto& element : uiElements_)
	{
		element->Update(cameraID);
	}
}

void EmptyScreen::Draw(int32_t renderTargetID)
{
	for (const auto& element : uiElements_)
	{
		element->Draw(renderTargetID);
	}
}
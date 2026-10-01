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
}

void EmptyScreen::Draw(int32_t renderTargetID)
{
}

void EmptyScreen::DrawImGui()
{

}

#include "PauseScreen.h"
#include <GameObjects/UI/UIElement/Pause/Pause.h>
#include <System/EventBus/EventBus.h>

PauseScreen::PauseScreen()
{
	elementTypes_.push_back(UIElementType::Pause);
}

PauseScreen::~PauseScreen()
{}

void PauseScreen::Initialize()
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

void PauseScreen::Update(int32_t cameraID)
{
	for (const auto& element : uiElements_)
	{
		element->Update(cameraID);
	}

	if (Game::IO::Key::IsJustPressed(VK_ESCAPE))
	{
		nextUIMode_ = UIMode::Playing;
	}
}

void PauseScreen::Draw(int32_t renderTargetID)
{
	for (const auto& element : uiElements_)
	{
		element->Draw(renderTargetID);
	}
}

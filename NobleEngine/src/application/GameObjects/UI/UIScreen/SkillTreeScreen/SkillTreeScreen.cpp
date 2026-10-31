#include "SkillTreeScreen.h"
#include <GameObjects/UI/UIElement/IUIElement.h>
#include <System/EventBus/EventBus.h>

SkillTreeScreen::SkillTreeScreen()
{
	elementTypes_.push_back(UIElementType::Hotbar);
	elementTypes_.push_back(UIElementType::MiningPointGauge);
	elementTypes_.push_back(UIElementType::SkillTree);
}

SkillTreeScreen::~SkillTreeScreen()
{}

void SkillTreeScreen::Initialize()
{
	nextUIMode_ = UIScreenType::MAX;

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

void SkillTreeScreen::Update(int32_t cameraID)
{
	for (const auto& element : uiElements_)
	{
		element->Update(cameraID);
	}

	if (Game::IO::Key::IsJustPressed(VK_ESCAPE) ||
		Game::IO::Key::IsJustPressed('E'))
	{
		nextUIMode_ = UIScreenType::Playing;
	}
}

void SkillTreeScreen::Draw(int32_t renderTargetID)
{
	for (const auto& element : uiElements_)
	{
		element->Draw(renderTargetID);
	}
}

void SkillTreeScreen::DrawImGui()
{
	for (const auto& element : uiElements_)
	{
		element->DrawImGui();
	}
}

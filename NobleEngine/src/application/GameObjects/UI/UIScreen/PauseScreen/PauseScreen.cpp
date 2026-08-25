#include "PauseScreen.h"
#include <GameObjects/UI/UIElement/Pause/Pause.h>
#include <GameObjects/Map/MapManager.h>

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

	// カーソル操作有効化
	Game::IO::Mouse::ShowCursor(true);
}

void PauseScreen::Update(int32_t cameraID)
{
	for (const auto& element : uiElements_)
	{
		element->Update(cameraID);
	}

	//// Pause要素の「セーブ要求」を処理
	//if (auto* pause = dynamic_cast<Pause*>(uiElements_[0].get()))
	//{
	//	if (pause->ConsumeSaveRequested())
	//	{
	//		mapManager_->SaveMap();
	//		Game::quit();
	//	}
	//}

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

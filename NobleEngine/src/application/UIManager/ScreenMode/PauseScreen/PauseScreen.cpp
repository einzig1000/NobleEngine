#include "PauseScreen.h"
#include "UIManager/ScreenMode/PauseScreen/PauseScreen.h"
#include "UIManager/ScreenMode/UIElement/Pause/Pause.h"
#include "MapManager/MapManager.h"

PauseScreen::PauseScreen()
{
	// uiElements_[0] : Pause
	uiElements_.emplace_back(std::make_unique<Pause>());
}

PauseScreen::~PauseScreen()
{}

void PauseScreen::Initialize()
{
	nextUIMode_ = UIMode::None;
	for (const auto& element : uiElements_)
	{
		element->Initialize();
	}
	// カーソル操作有効化
	Game::IO::Mouse::ShowCursor(true);
}

void PauseScreen::Update()
{
	for (const auto& element : uiElements_)
	{
		element->Update();
	}

	// Pause要素の「セーブ要求」を処理
	if (auto* pause = dynamic_cast<Pause*>(uiElements_[0].get()))
	{
		if (pause->ConsumeSaveRequested())
		{
			mapManager_->SaveMap();
			Game::quit();
		}
	}

	if (Game::IO::Key::IsJustPressed(DIK_ESCAPE))
	{
		nextUIMode_ = UIMode::Playing;
	}
}

void PauseScreen::Draw()
{
	for (const auto& element : uiElements_)
	{
		element->Draw();
	}
}

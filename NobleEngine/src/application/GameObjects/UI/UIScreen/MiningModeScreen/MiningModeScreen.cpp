#include "MiningModeScreen.h"
#include <GameObjects/UI/UIElement/MiningMode/MiningMode.h>

MiningModeScreen::MiningModeScreen()
{
	elementTypes_.push_back(UIElementType::MiningMode);
}

MiningModeScreen::~MiningModeScreen()
{}

void MiningModeScreen::Initialize()
{
	nextUIMode_ = UIMode::None;

	for (const auto& element : uiElements_)
	{
		element->Initialize();
	}

	// カメラ操作を無効化
}

void MiningModeScreen::Update(int32_t cameraID)
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

void MiningModeScreen::Draw(int32_t renderTargetID)
{
	for (const auto& element : uiElements_)
	{
		element->Draw(renderTargetID);
	}
}

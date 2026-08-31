#include "Application.h"
#include <GameManager/GameManager.h>
#include <ResourceLoader/Data/DataManager.h>
#include <Editor/editor.h>

Application& Application::Instance()
{
	static Application instance;
	return instance;
}

void Application::Initialize()
{
	// Font読み込み
	Game::Asset::Font::Load("Assets/engine/fonts/DotGothic16/DotGothic16-Regular.ttf");
	
	dataManager_ = std::make_unique<DataManager>();
	editor_ = std::make_unique<Editor>(dataManager_.get());


	gameManager_ = std::make_unique<GameManager>();
}

void Application::Update()
{
	editor_->Update();

	gameManager_->Update();
}

void Application::Draw()
{
	editor_->Draw();

	gameManager_->Draw();
}

void Application::DrawImGui()
{
	editor_->DrawImGui();

	gameManager_->DrawImGui();
}

void Application::Finalize()
{
	gameManager_.reset();
	editor_.reset();
	dataManager_.reset();
}

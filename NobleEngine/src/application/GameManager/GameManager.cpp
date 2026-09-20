#include "GameManager.h"
#include <App.h>
#include <Utilities/Logger/Logger.h>
#include <GameManager/Phase/TitlePhase/TitlePhase.h>
#include <GameManager/Phase/GameScenePhase/GameScenePhase.h>
#include <GameManager/Phase/TestPhase/TestPhase.h>
#include <Utilities/Json/JsonManager.h>


GameManager::GameManager() 
{
	currentPhase_ = CreatePhase(Phase::Phase_GameScene);
	//currentPhase_ = CreatePhase(Phase::Phase_Test);
	currentPhase_->SetContext(&phaseContext_);
	currentPhase_->Initialize();

	JsonManager::LoadAll("assets/application/json");
}

GameManager::~GameManager()
{
}


void GameManager::Update()
{
	if (currentPhase_->GetNextPhase() != Phase::Phase_None)
	{
		currentPhase_ = CreatePhase(currentPhase_->GetNextPhase());
		currentPhase_->SetContext(&phaseContext_); 
		currentPhase_->Initialize();
	}
	currentPhase_->Update();

	if (Game::IO::Key::IsJustPressed(VK_F11))
	{
		Game::Asset::RenderTexture::SaveAllRenderTextureToFile("generated/screenshots");
	}
}

void GameManager::Draw()
{
	currentPhase_->Draw();

}

void GameManager::DrawImGui()
{
	currentPhase_->DrawImGui();

}


std::unique_ptr<IPhase> GameManager::CreatePhase(Phase phase)
{
	switch (phase)
	{
	case Phase::Phase_Title:
		return std::make_unique<TitlePhase>();
	case Phase::Phase_GameScene:
		return std::make_unique<GameScenePhase>();
	case Phase::Phase_Test:
		return std::make_unique<TestPhase>();

	default:
		Log("Error : 該当するフェーズクラスが存在しません");
		assert(false);
		return nullptr;
	}
}
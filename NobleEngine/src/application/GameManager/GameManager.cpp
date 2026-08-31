#include "GameManager.h"
#include <App.h>
#include <Utilities/Logger/Logger.h>
#include <GameManager/Phase/TitlePhase/TitlePhase.h>
#include <GameManager/Phase/GameScenePhase/GameScenePhase.h>
#include <Utilities/Json/JsonManager.h>


GameManager::GameManager() 
{
	//currentPhase_ = CreatePhase(PHASE::Phase_GameScene);
	currentPhase_ = CreatePhase(PHASE::Phase_Title);
	currentPhase_->SetContext(&phaseContext_);
	currentPhase_->Initialize();

	JsonManager::LoadAll("assets/application/json");
}

GameManager::~GameManager()
{
}


void GameManager::Update()
{
	if (currentPhase_->GetNextPhase() != PHASE::Phase_None)
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


std::unique_ptr<IPhase> GameManager::CreatePhase(PHASE phase)
{
	switch (phase)
	{
	case PHASE::Phase_Title:
		return std::make_unique<TitlePhase>();
	case PHASE::Phase_GameScene:
		return std::make_unique<GameScenePhase>();
	default:
		Log("Error : 該当するフェーズクラスが存在しません");
		assert(false);
		return nullptr;
	}
}
#include "GameManager.h"
#include <Utilities/Logger/Logger.h>
#include "Phase/TestPhase/TestPhase.h"
#include "Phase/BattlePhase/BattlePhase.h"
#include "Phase/TitlePhase/TitlePhase.h"
#include "Phase/GameScenePhase/GameScenePhase.h"

GameManager::GameManager()
{
	currentPhase_ = CreatePhase(PHASE::Phase_Test);
	currentPhase_->SetContext(&phaseContext_);
	currentPhase_->Initialize();
}

GameManager::~GameManager()
{

}

void GameManager::Update()
{
	currentPhase_->Update();
	if (currentPhase_->GetNextPhase() != PHASE::Phase_None)
	{
		currentPhase_ = CreatePhase(currentPhase_->GetNextPhase());
		currentPhase_->SetContext(&phaseContext_);
		currentPhase_->Initialize();
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

std::unique_ptr<PhaseParent> GameManager::CreatePhase(PHASE phase)
{
	switch (phase)
	{
	case PHASE::Phase_Test:
		return std::make_unique<TestPhase>();
	case PHASE::Phase_Title:
		return std::make_unique<TitlePhase>();
	case PHASE::Phase_GameScene:
		//return std::make_unique<GameScenePhase>();
	default:
		assert(false);
		Log("Error : 該当するフェーズクラスが存在しません");
		return nullptr;
	}
}
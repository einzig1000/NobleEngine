#include "GameManager.h"
#include <App.h>
#include <Utilities/Logger/Logger.h>
#include <GameManager/Phase/TitlePhase/TitlePhase.h>
#include <GameManager/Phase/GameScenePhase/GameScenePhase.h>
#include <GameManager/Phase/TestPhase/TestPhase.h>
#include <GameManager/Phase/ShaderToyTestPhase/ShaderToyTestPhase.h>
#include <Utilities/Json/JsonManager.h>


GameManager::GameManager() 
{
	currentPhase_ = Phase::Phase_None;
	nextPhase_ = Phase::Phase_ShaderToyTest;
	nextPhase_ = Phase::Phase_GameScene;

	//JsonManager::LoadAll("assets/application/json");
}

GameManager::~GameManager()
{
}


void GameManager::Update()
{
	if (nextPhase_ != Phase::Phase_None)
	{
		phaseInstance_ = CreatePhase(nextPhase_);
		phaseInstance_->SetContext(&phaseContext_); 
		phaseInstance_->Initialize();
		currentPhase_ = nextPhase_;
		nextPhase_ = Phase::Phase_None;
	}

	phaseInstance_->Update();

	if (phaseInstance_->GetNextPhase() != Phase::Phase_None)
	{
		nextPhase_ = phaseInstance_->GetNextPhase();
	}

	if (Game::IO::Key::IsJustPressed(VK_F11))
	{
		if (Game::IO::Key::IsHeld(VK_SHIFT))
		{
			Game::Asset::RenderTexture::SaveAllRenderTextureToFile("screenshots");
		}
		else
		{
			Game::Asset::RenderTexture::SaveRenderTextureToFile("screenshots", "NobleScreen");
		}
	}

	if (Game::IO::Key::IsJustPressed(VK_F10))
	{
		Game::Window::ToggleFullscreen();
	}

	if (Game::IO::Key::IsJustPressed(VK_F3))
	{
		Game::System::ToggleDrawImGui();
	}
}

void GameManager::Draw()
{
	phaseInstance_->Draw();
}

void GameManager::DrawImGui()
{
	phaseInstance_->DrawImGui();


	ImGui::Begin("GameManager");
	

	auto values = magic_enum::enum_values<Phase>();
	auto names = magic_enum::enum_names<Phase>();
	if (ImGui::BeginCombo("Phase", magic_enum::enum_name(currentPhase_).data()))
	{
		for (std::size_t i = 0; i < values.size(); i++)
		{
			Phase value = values[i];
			bool selected = (currentPhase_ == value);

			if (ImGui::Selectable(names[i].data(), selected))
			{
				nextPhase_ = value;
			}
			if (selected) ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::End();
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
	case Phase::Phase_ShaderToyTest:
		return std::make_unique<ShaderToyTestPhase>();

	default:
		Log("Error : 該当するフェーズクラスが存在しません");
		assert(false);
		return nullptr;
	}
}
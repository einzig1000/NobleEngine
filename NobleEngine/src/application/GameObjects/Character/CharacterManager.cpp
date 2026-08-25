#include "CharacterManager.h"

CharacterManager::CharacterManager(MapManager* mapManager)
	: mapManager_(mapManager)
{
	enemies_.clear();

	player_ = std::make_unique<Player>();
	player_->SetMapManager(mapManager_);
	player_->Initialize();
}

CharacterManager::~CharacterManager()
{}

void CharacterManager::Initialize()
{
	enemies_.clear();
}

void CharacterManager::SetViewCamera(int32_t cameraID)
{
	player_->SetViewCamera(cameraID);
}

void CharacterManager::Update(int32_t cameraID)
{
	player_->Update(cameraID);


	for (auto& enemy : enemies_)
	{
		enemy->Update(cameraID);
	}
}

void CharacterManager::Draw(int32_t renderTextureID)
{
	player_->Draw(renderTextureID);

	for (auto& enemy : enemies_)
	{
		enemy->Draw(renderTextureID);
	}
}

void CharacterManager::DrawImGui()
{
	player_->DrawImGui();

	for (auto& enemy : enemies_)
	{
		enemy->DrawImGui();
	}
}

void CharacterManager::AddEnemy(std::unique_ptr<ICharacter> character)
{
	character->SetID(static_cast<int32_t>(enemies_.size()));
	character->SetMapManager(mapManager_);
	character->SetEventBus(eventBus_);
	character->Initialize();
	enemies_.push_back(std::move(character));
}

void CharacterManager::RemoveEnemy(int32_t enemyID)
{
	// 最後尾とswapしてから最後尾を削除する
	enemies_.back()->SetID(enemyID);
	enemies_[enemyID] = std::move(enemies_.back());
	enemies_.pop_back();
}

void CharacterManager::SetEventBus(EventBus* eventBus)
{
	player_->SetEventBus(eventBus);
	eventBus_ = eventBus;
}

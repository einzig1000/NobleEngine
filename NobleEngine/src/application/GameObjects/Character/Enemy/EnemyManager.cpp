#include "Character/Enemy/EnemyManager.h"
#include "Character/Enemy/Enemy.h"
#include "Character/Player/Player.h"
#include "MapManager/MapManager.h"


EnemyManager::EnemyManager()
{}

EnemyManager::~EnemyManager()
{}

void EnemyManager::Initialize()
{}

void EnemyManager::Update()
{
	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}

	spawnTimer_++;

	//if (spawnTimer_ % 420 == 0)
	//{
	//	// 敵スポーン
	//
	//	// 出現場所はプレイヤーの視線後ろ方向
	//	Vector3 spawnPos = playerptr_->data_.translate.value;
	//	Ray playerRay = playerptr_->viewRay_;
	//
	//	Vector3 backDir = playerRay.diff.Normalized() * -1.0f;
	//	spawnPos += backDir * 10.0f;
	//
	//	while (mapManager_->isSolidAt(spawnPos))
	//	{
	//		spawnPos.y += 1.0f;
	//	}
	//
	//	AddNewEnemy(spawnPos);
	//}
}

void EnemyManager::Draw()
{
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}
}

void EnemyManager::DrawImGui()
{
	for (auto& enemy : enemies_)
	{
		enemy->DrawImGui();
	}
}

void EnemyManager::AddNewEnemy(Vector3 pos)
{
	auto enemy = std::make_unique<Enemy>(pos);
	enemy->SetPlayerPtr(playerptr_);
	enemy->SetMapManager(mapManager_);
	enemy->SetUIManager(uiManager_);
	enemy->Initialize();
	mapManager_->RegisterCharacter(enemy.get());
	enemies_.emplace_back(std::move(enemy));
}

#pragma once
#include "Game.h"

class Enemy;
class Player;
class MapManager;
class UIManager;

class EnemyManager
{
public:
	EnemyManager();
	void SetPlayer(Player* player) { playerptr_ = player; }
	void SetMapManager(MapManager* mapManager) { mapManager_ = mapManager; }
	void SetUIManager(UIManager* uiManager) { uiManager_ = uiManager; }
	~EnemyManager();
	void Initialize();
	void Update();
	void Draw();
	void DrawImGui();
	void AddNewEnemy(Vector3 pos);

private:
	std::vector<std::unique_ptr<Enemy>> enemies_;

	Player* playerptr_;
	MapManager* mapManager_;
	UIManager* uiManager_;

	int32_t spawnTimer_ = 0;

};


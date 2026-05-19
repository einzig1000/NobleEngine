#pragma once
#include "GameManager/Phase/PhaseParent/PhaseParent.h"
#include "MapManager/Chunk/Block/Block.h"

class Player;
class MapManager;
class MapWorldCollider;
class CameraController;
class UIManager;
class EnemyManager;

class GameScenePhase :
	public PhaseParent
{
public:
	GameScenePhase();
	~GameScenePhase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(PHASE phase) override { nextPhase_ = phase; }


private:

	// カメラ
	std::unique_ptr<CameraController> cameraController_;

	// プレイヤー
	std::unique_ptr<Player> player_;

	// 敵マネージャー
	std::unique_ptr<EnemyManager> enemyManager_;

	// マップ
	std::unique_ptr<MapManager> map_;

	// UIマネージャー
	std::unique_ptr<UIManager> uiManager_;

};
#pragma once
#include <GameManager/Phase/IPhase.h>
#include <memory>

class Player;
class MapManager;
class SkyBox;
class CameraController;
class UIManager;
class EnemyManager;
class ScreenDrawer;
class CharacterManager;
class EventBus;

class GameScenePhase :
	public IPhase
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
	// カメラID
	int32_t c_debug_ = -1;
	int32_t c_player_ = -1;

	// イベントバス
	std::unique_ptr<EventBus> eventBus_;


	// カメラ
	std::unique_ptr<CameraController> cameraController_;

	// 描画マネージャ
	std::unique_ptr<ScreenDrawer> screenDrawer_;


	// キャラクターマネージャー
	std::unique_ptr<CharacterManager> charctorManager_;

	// マップ
	std::unique_ptr<MapManager> map_;

	// UIマネージャー
	std::unique_ptr<UIManager> uiManager_;

};
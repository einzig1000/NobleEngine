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
class EffectManager;
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
	void ChangePhase(Phase phase) override { nextPhase_ = phase; }

private:

	// イベントバス
	std::unique_ptr<EventBus> eventBus_;

	// カメラ
	std::unique_ptr<CameraController> cameraController_;
	int32_t c_player_ = -1;

	// 描画マネージャ
	std::unique_ptr<ScreenDrawer> screenDrawer_;

	// エフェクトマネージャ
	std::unique_ptr<EffectManager> effectManager_;

	// キャラクターマネージャー
	std::unique_ptr<CharacterManager> charcterManager_;

	// マップ
	std::unique_ptr<MapManager> map_;

	// UIマネージャー
	std::unique_ptr<UIManager> uiManager_;

};
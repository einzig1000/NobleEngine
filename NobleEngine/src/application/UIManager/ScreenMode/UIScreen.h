#pragma once
#include <vector>
#include <memory>
#include "Game.h"

class UIElement;
class Player;
class MapManager;

class UIScreen
{
public:
	UIScreen() = default;
	virtual ~UIScreen() = default;
	// 画面の初期化
	virtual void Initialize() = 0;
	// 画面の更新
	virtual void Update() = 0;
	// 画面の描画
	virtual void Draw() = 0;

	virtual UIMode GetNextUIMode() const { return nextUIMode_; }

	// プレイヤーセット
	void SetPlayer(Player* player) { player_ = player; }
	// マップマネージャーセット
	void SetMapManager(MapManager* mapManager) { mapManager_ = mapManager; }


protected:
	UIMode nextUIMode_ = UIMode::None;
	std::vector<std::unique_ptr<UIElement>> uiElements_;

	Player* player_ = nullptr;
	MapManager* mapManager_ = nullptr;
};


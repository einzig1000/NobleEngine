#pragma once
#include "Game.h"
#include <memory>

class Player;
class UIScreen;
class PlayingScreen;
class InventoryScreen;
class CraftScreen;
class PauseScreen;
class MapManager;

class UIManager
{
public:
	UIManager();
	void SetPlayer(Player* player) { player_ = player; }
	void SetMapManager(MapManager* mapManager) { mapManager_ = mapManager; }
	~UIManager();
	void Initialize();
	void Update();
	void Draw();
	void DrawImGui();

	void ChangeScreen(UIMode mode);

	UIMode GetCurrentUIMode() const { return currentUIMode_; }

private:
	UIMode currentUIMode_ = UIMode::None;

	UIScreen* currentScreen_ = nullptr;

	std::unique_ptr<PlayingScreen> playingScreen_ = nullptr;
	std::unique_ptr<InventoryScreen> inventoryScreen_ = nullptr;
	std::unique_ptr<CraftScreen> craftingScreen_ = nullptr;
	std::unique_ptr<PauseScreen> pauseScreen_ = nullptr;

	Player* player_ = nullptr;
	MapManager* mapManager_ = nullptr;
};


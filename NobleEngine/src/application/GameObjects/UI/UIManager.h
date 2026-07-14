#pragma once
#include <Game.h>
#include <memory>
#include "ScreenMode/PlayingScreen/PlayingScreen.h"
#include "ScreenMode/InventoryScreen/InventoryScreen.h"
#include "ScreenMode/CraftScreen/CraftScreen.h"
#include "ScreenMode/PauseScreen/PauseScreen.h"

class Player;
class MapManager;
class UIScreen;

/////////////// UIManagerが全エレメントを持って、各シーンに渡して共有する方法が綺麗では？


class UIManager
{
public:
	UIManager();
	void SetPlayer(Player* player);
	void SetMapManager(MapManager* mapManager);
	~UIManager();

	void Initialize();
	void Update(int32_t cameraID);
	void Draw();
	void DrawImGui();

	void ChangeScreen(UIMode mode);

	UIMode GetCurrentUIMode() const { return currentUIMode_; }

private:
	UIMode currentUIMode_ = UIMode::None;

	UIScreen* currentScreen_ = nullptr;

	std::unique_ptr<PlayingScreen> playingScreen_;
	std::unique_ptr<InventoryScreen> inventoryScreen_;
	std::unique_ptr<CraftScreen> craftingScreen_;
	std::unique_ptr<PauseScreen> pauseScreen_;
};


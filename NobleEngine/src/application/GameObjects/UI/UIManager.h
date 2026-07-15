#pragma once
#include <Game.h>
#include <memory>
#include "UIScreen/IUIScreen.h"
#include "UIElement/IUIElement.h"
#include "UIData.h"

class Player;
class MapManager;

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
	void Draw(int32_t renderTargetID);
	void DrawImGui();

	void ChangeScreen(UIMode mode);

	UIMode GetCurrentUIMode() const { return currentUIMode_; }

private:
	UIMode currentUIMode_ = UIMode::None;
	IUIScreen* currentScreen_ = nullptr;

	std::unordered_map<UIMode, std::unique_ptr<IUIScreen>> screens_;
	std::unordered_map<UIElementType, std::unique_ptr<IUIElement>> elements_;
};


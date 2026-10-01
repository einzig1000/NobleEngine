#pragma once
#include <Game.h>
#include <memory>
#include "UIScreen/IUIScreen.h"
#include "UIElement/IUIElement.h"
#include "UIData.h"

class EventBus;
class ItemInventory;
class MiningPointGauge;
class ItemIconManager;

class UIManager
{
public:
	UIManager();
	~UIManager();

	void Initialize();
	void Update(int32_t cameraID);
	void Draw(int32_t renderTargetID);
	void DrawImGui();

	void ChangeScreen(UIMode mode);

	void SetEventBus(EventBus* eventBus);
	void SetInventory(const ItemInventory* inventory);
	void SetMiningGauge(const MiningPointGauge* miningGauge);

private:
	EventBus* eventBus_ = nullptr;

	size_t currentUIMode_ = 0;
	IUIScreen* currentScreen_ = nullptr;

	std::vector<std::unique_ptr<IUIScreen>> screens_;
	std::vector<std::unique_ptr<IUIElement>> elements_;

	// アイテムのアイコン(モデルから作る)
	std::unique_ptr<ItemIconManager> iconManager_;
};

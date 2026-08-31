#pragma once
#include <Game.h>
#include <memory>
#include "UIScreen/IUIScreen.h"
#include "UIElement/IUIElement.h"
#include "UIData.h"

class EventBus;
class ItemInventory;

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

private:
	EventBus* eventBus_ = nullptr;
	const ItemInventory* inventory_ = nullptr;

	size_t currentUIMode_ = 0;
	IUIScreen* currentScreen_ = nullptr;

	std::vector<std::unique_ptr<IUIScreen>> screens_;
	std::vector<std::unique_ptr<IUIElement>> elements_;
};


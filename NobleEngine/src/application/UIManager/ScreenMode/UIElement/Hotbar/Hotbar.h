#pragma once
#include "Game.h"
#include "UIManager/ScreenMode/UIElement/UIElement.h"

class Hotbar : public UIElement
{
	public:
	Hotbar();
	~Hotbar() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;

	int32_t selectedIndex_ = 0;
};


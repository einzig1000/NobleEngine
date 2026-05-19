#pragma once
#include "Game.h"
#include "UIManager/ScreenMode/UIScreen.h"

class InventoryScreen : public UIScreen
{
public:
	InventoryScreen();
	~InventoryScreen() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;

};


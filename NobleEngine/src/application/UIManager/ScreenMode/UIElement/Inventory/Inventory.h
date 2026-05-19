#pragma once
#include "Game.h"
#include "UIManager/ScreenMode/UIElement/UIElement.h"

class Inventory : public UIElement
{
public:
	Inventory();
	~Inventory() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;


};


#pragma once
#include "Game.h"
#include "UIManager/ScreenMode/UIElement/UIElement.h"

class Craft : public UIElement
{
public:
	Craft();
	~Craft() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;


};


#pragma once
#include "Game.h"
#include "UIManager/ScreenMode/UIScreen.h"

class PauseScreen : public UIScreen
{
public:
	PauseScreen();
	~PauseScreen() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
};


#pragma once
#include "Game.h"
#include "UIManager/ScreenMode/UIScreen.h"

class PlayingScreen : public UIScreen
{
	public:
	PlayingScreen();
	~PlayingScreen() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
};


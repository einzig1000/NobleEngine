#pragma once
#include "Game.h"
#include "UIManager/ScreenMode/UIScreen.h"

class CraftScreen : public UIScreen
{
public:
	CraftScreen();
	~CraftScreen() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;


};


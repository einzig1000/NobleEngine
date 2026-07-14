#pragma once
#include <Game.h>
#include <GameObjects/UI/ScreenMode/UIScreen.h>

class CraftScreen : public UIScreen
{
public:
	CraftScreen();
	~CraftScreen() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw() override;


};


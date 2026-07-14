#pragma once
#include <Game.h>
#include <GameObjects/UI/ScreenMode/UIScreen.h>

class PauseScreen : public UIScreen
{
public:
	PauseScreen();
	~PauseScreen() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw() override;
};


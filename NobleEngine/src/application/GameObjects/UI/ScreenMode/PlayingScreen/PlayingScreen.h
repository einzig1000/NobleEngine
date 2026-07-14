#pragma once
#include <Game.h>
#include <GameObjects/UI/ScreenMode/UIScreen.h>

class PlayingScreen : public UIScreen
{
	public:
	PlayingScreen();
	~PlayingScreen() override;
	void Initialize() override;	
	void Update(int32_t cameraID) override;
	void Draw() override;
};


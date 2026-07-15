#pragma once
#include <Game.h>
#include <GameObjects/UI/UIScreen/IUIScreen.h>

class PauseScreen : public IUIScreen
{
public:
	PauseScreen();
	~PauseScreen() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t renderTargetID) override;
};


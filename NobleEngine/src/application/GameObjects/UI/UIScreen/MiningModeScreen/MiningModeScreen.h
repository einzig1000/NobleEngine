#pragma once
#include <Game.h>
#include <GameObjects/UI/UIScreen/IUIScreen.h>

class MiningModeScreen : public IUIScreen
{
public:
	MiningModeScreen();
	~MiningModeScreen() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t renderTargetID) override;


};


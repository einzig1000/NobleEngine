#pragma once
#include <Game.h>
#include <GameObjects/UI/UIScreen/IUIScreen.h>

class EmptyScreen : public IUIScreen
{
public:
	EmptyScreen();
	~EmptyScreen() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t renderTargetID) override;
};


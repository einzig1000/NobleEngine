#pragma once
#include <Game.h>
#include <GameObjects/UI/UIScreen/IUIScreen.h>

class InventoryScreen : public IUIScreen
{
public:
	InventoryScreen();
	~InventoryScreen() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t renderTargetID) override;

};


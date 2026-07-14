#pragma once
#include <Game.h>
#include <GameObjects/UI/UIElement/UIElement.h>

class Hotbar : public UIElement
{
public:
	Hotbar();
	~Hotbar() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t rt_ID) override;

	int32_t selectedIndex_ = 0;
};


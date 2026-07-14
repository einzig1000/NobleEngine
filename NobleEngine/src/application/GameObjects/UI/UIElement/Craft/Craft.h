#pragma once
#include <Game.h>
#include <GameObjects/UI/UIElement/UIElement.h>

class Craft : public UIElement
{
public:
	Craft();
	~Craft() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t rt_ID) override;


};


#pragma once
#include <Game.h>
#include <GameObjects/UI/UIElement/IUIElement.h>

class Craft : public IUIElement
{
public:
	Craft();
	~Craft() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t rt_ID) override;


};


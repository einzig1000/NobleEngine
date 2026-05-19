#pragma once
#include "Game.h"
#include "UIManager/ScreenMode/UIElement/UIElement.h"

class MapManager;

class Pause : public UIElement
{
public:
	Pause();
	~Pause() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;


	bool ConsumeSaveRequested();
private:
	bool saveRequested_ = false;
};


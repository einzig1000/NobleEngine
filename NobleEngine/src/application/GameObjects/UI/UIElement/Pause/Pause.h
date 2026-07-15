#pragma once
#include <Game.h>
#include <GameObjects/UI/UIElement/IUIElement.h>


class Pause : public IUIElement
{
public:
	Pause();
	~Pause() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t rt_ID) override;

	bool ConsumeSaveRequested();

private:
	bool saveRequested_ = false;

	Vector2 buttonSize_;
};


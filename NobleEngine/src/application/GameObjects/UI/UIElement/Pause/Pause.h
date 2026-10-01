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
	void DrawImGui() override;

private:
	Matrix4x4 orthographic_;

	Vector2 buttonSize_;
};


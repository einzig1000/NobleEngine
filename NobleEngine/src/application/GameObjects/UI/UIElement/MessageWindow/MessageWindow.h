#pragma once
#include <Game.h>
#include <definition/definition.h>
#include <GameObjects/UI/UIElement/IUIElement.h>
#include <array>
#include <string>


class MessageWindow : public IUIElement
{
public:
	MessageWindow();
	~MessageWindow() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t rt_ID) override;
	void DrawImGui() override;

	void SetMessage(const std::string& message);

private:

	Matrix4x4 orthographic_;
};


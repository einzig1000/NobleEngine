#pragma once
#include <GameObjects/UI/UIElement/IUIElement.h>

class Hotbar : public IUIElement
{
public:
	Hotbar();
	~Hotbar() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t rt_ID) override;
	void DrawImGui() override;

	Vector3 GetSlotPosition(int32_t index) const;


	int32_t selectedIndex_ = 0;

private:
	Matrix4x4 orthographic_;

	std::vector<ElementData> icons_;
};


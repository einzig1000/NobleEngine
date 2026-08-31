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

	Vector3 GetSlotPosition(int32_t index) const;

	std::vector<ElementData> icons_;

	int32_t selectedIndex_ = 0;
};


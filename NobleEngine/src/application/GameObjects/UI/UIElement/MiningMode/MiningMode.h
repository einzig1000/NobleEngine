#pragma once
#include <GameObjects/UI/UIElement/IUIElement.h>

class MiningMode : public IUIElement
{
public:
	MiningMode();
	~MiningMode() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t rt_ID) override;
};


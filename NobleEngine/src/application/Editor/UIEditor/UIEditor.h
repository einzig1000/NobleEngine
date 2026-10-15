#pragma once
#include <GameObjects/UI/UIData.h>
#include <Game.h>

class UIEditor
{
public:
	UIEditor();
	~UIEditor();
	void Update();
	void Draw();
	void DrawImGui();


private:

	int32_t rt_ID = -1;
	int32_t c_ID = -1;

	std::vector<ElementData> elementData_;
	int32_t selectedElementIndex_ = -1;

	UIElementType elementType_ = UIElementType::MAX;
	UIScreenType screenType_ = UIScreenType::MAX;
};


#pragma once
#include "ItemEditor/ItemEditor.h"
#include "UIEditor/UIEditor.h"

class DataManager;

class Editor
{
public:
	Editor();
	~Editor();

	void Update();
	void Draw();
	void DrawImGui();

private:
	ItemEditor itemEditor_;
	UIEditor uiEditor_;
};


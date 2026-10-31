#pragma once
#include "ItemEditor/ItemEditor.h"
#include "UIEditor/UIEditor.h"
#include "SkillTreeEditor/SkillTreeEditor.h"

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
	SkillTreeEditor skillTreeEditor_;
};

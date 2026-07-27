#pragma once
#include "ItemEditor/ItemEditor.h"
#include <memory>

class DataManager;

class Editor
{
public:
	Editor(DataManager* dataManager);
	~Editor();

	void Update();
	void Draw();
	void DrawImGui();

private:
	std::unique_ptr<ItemEditor> itemEditor_;
};


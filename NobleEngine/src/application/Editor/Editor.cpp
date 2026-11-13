#include "Editor.h"

Editor::Editor()
{}

Editor::~Editor()
{}

void Editor::Update()
{
	itemEditor_.Update();
	skillTreeEditor_.Update();
	//uiEditor_.Update();
}

void Editor::Draw()
{
	itemEditor_.Draw();
	skillTreeEditor_.Draw();
	//uiEditor_.Draw();
}

void Editor::DrawImGui()
{
	itemEditor_.DrawImGui();
	skillTreeEditor_.DrawImGui();
	//uiEditor_.DrawImGui();
}

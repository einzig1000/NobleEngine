#include "Editor.h"

Editor::Editor(DataManager* dataManager)
{
	itemEditor_ = std::make_unique<ItemEditor>(dataManager);
}

Editor::~Editor()
{
	itemEditor_.reset();
}

void Editor::Update()
{
	itemEditor_->Update();
}

void Editor::Draw()
{
	itemEditor_->Draw();
}

void Editor::DrawImGui()
{
	itemEditor_->DrawImGui();
}

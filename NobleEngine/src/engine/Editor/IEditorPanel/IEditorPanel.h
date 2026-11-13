#pragma once


class IEditorPanel
{
public:
	virtual ~IEditorPanel() = default;

	virtual void Update() {}
	virtual void Draw() {}
	virtual void DrawImGui() = 0;
};
#pragma once

class FixFPS;

class TimeEditor
{
public:
	TimeEditor(FixFPS* fixFPS);
	~TimeEditor();
	void DrawImGui();

private:
	FixFPS* fixFPS_ = nullptr;

};


#pragma once

class FixFPS;
class FrameProfiler;

class TimeEditor
{
public:
	TimeEditor(FixFPS* fixFPS, FrameProfiler* frameProfiler);
	~TimeEditor();
	void DrawImGui();

private:
	FixFPS* fixFPS_ = nullptr;
	FrameProfiler* frameProfiler_ = nullptr;

};


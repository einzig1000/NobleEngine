#pragma once

class TimeManager;
class FrameProfiler;

class TimeEditor
{
public:
	TimeEditor(TimeManager* timeManager, FrameProfiler* frameProfiler);
	~TimeEditor();
	void DrawImGui();

private:
	TimeManager* timeManager_ = nullptr;
	FrameProfiler* frameProfiler_ = nullptr;

};


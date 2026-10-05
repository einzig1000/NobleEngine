#pragma once
#include <cstdint>

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

	int32_t targetFPSCap = 0;
	float timeScale = 0.0f;
};


#pragma once
#include <cstdint>
#include <Editor/IEditorPanel/IEditorPanel.h>

class TimeManager;
class FrameProfiler;

class TimeEditor : public IEditorPanel
{
public:
	TimeEditor(TimeManager* timeManager, FrameProfiler* frameProfiler);
	~TimeEditor();
	void DrawImGui() override;

private:
	TimeManager* timeManager_ = nullptr;
	FrameProfiler* frameProfiler_ = nullptr;

	int32_t targetFPSCap = 0;
	float timeScale = 0.0f;
};


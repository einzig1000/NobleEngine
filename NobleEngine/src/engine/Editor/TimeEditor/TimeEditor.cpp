#include "TimeEditor.h"
#include <TimeManager/TimeManager.h>
#include <ImGuiManager/ImGuiManager.h>
#include <DirectX/FrameProfiler/FrameProfiler.h>

TimeEditor::TimeEditor(TimeManager* timeManager, FrameProfiler* frameProfiler)
	: timeManager_(timeManager), frameProfiler_(frameProfiler)
{}

TimeEditor::~TimeEditor()
{}

void TimeEditor::DrawImGui()
{
    ImGui::Begin("Time Editor");
    ImGui::Text("Clamped FPS    : %5.1f  (%4.1f ms)", timeManager_->GetFixFPS()->GetClampedFPS(), timeManager_->GetFixFPS()->GetClampedDeltaTimeMs());
    ImGui::Text("Unclamped FPS  : %5.1f  (%4.1f ms)", timeManager_->GetFixFPS()->GetUnclampedFPS(), timeManager_->GetFixFPS()->GetUnclampedDeltaTimeMs());
    ImGui::Text("CPU FPS        : %5.1f  (%4.1f ms)", frameProfiler_->GetCpuFPS(), frameProfiler_->GetCpuDeltaTimeMs());
    ImGui::Text("GPU FPS        : %5.1f  (%4.1f ms)", frameProfiler_->GetGpuFPS(), frameProfiler_->GetGpuDeltaTimeMs());
    ImGui::Text("Frame Count    : %d", timeManager_->GetFixFPS()->GetFrameCount());
	static int32_t targetFPSCap = 60;
    if (ImGui::DragInt("Target fpsCap", &targetFPSCap, 1.0f, 1, 60))
    {
        timeManager_->GetFixFPS()->SetFPSCap(targetFPSCap);
    }
	static float timeScale = 1.0f;
	if (ImGui::DragFloat("Time Scale", &timeScale, 0.01f, 0.0f, 100.0f))
	{
		timeManager_->SetTimeScale(timeScale);
	}

    ImGui::End();
}

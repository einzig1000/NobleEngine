#include "TimeEditor.h"
#include <FixFPS/FixFPS.h>
#include <ImGuiManager/ImGuiManager.h>
#include <DirectX/FrameProfiler/FrameProfiler.h>

TimeEditor::TimeEditor(FixFPS* fixFPS, FrameProfiler* frameProfiler)
	: fixFPS_(fixFPS), frameProfiler_(frameProfiler)
{}

TimeEditor::~TimeEditor()
{}

void TimeEditor::DrawImGui()
{
    ImGui::Begin("Time Editor");
    ImGui::Text("Clamped FPS    : %5.1f  (%4.1f ms)", fixFPS_->GetClampedFPS(), fixFPS_->GetClampedDeltaTimeMs());
    ImGui::Text("Unclamped FPS  : %5.1f  (%4.1f ms)", fixFPS_->GetUnclampedFPS(), fixFPS_->GetUnclampedDeltaTimeMs());
    ImGui::Text("CPU FPS        : %5.1f  (%4.1f ms)", frameProfiler_->GetCpuFPS(), frameProfiler_->GetCpuDeltaTimeMs());
    ImGui::Text("GPU FPS        : %5.1f  (%4.1f ms)", frameProfiler_->GetGpuFPS(), frameProfiler_->GetGpuDeltaTimeMs());
    ImGui::Text("Frame Count    : %d", fixFPS_->GetFrameCount());
	static int32_t targetFPSCap = 60;
    if (ImGui::DragInt("Target fpsCap", &targetFPSCap, 1.0f, 1, 60))
    {
        fixFPS_->SetFPSCap(targetFPSCap);
    }
    ImGui::End();
}

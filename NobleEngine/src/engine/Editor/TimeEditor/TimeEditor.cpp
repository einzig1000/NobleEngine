#include "TimeEditor.h"
#include <FixFPS/FixFPS.h>
#include <ImGuiManager/ImGuiManager.h>

TimeEditor::TimeEditor(FixFPS* fixFPS)
	: fixFPS_(fixFPS)
{}

TimeEditor::~TimeEditor()
{}

void TimeEditor::DrawImGui()
{
	ImGui::Begin("Time Editor");
	ImGui::Text("FPS		: %.1f ", fixFPS_->GetRealFPS());
	ImGui::Text("Max FPS	: %.1f ", fixFPS_->GetMaxFPS());
	ImGui::Text("deltaTime	: %.4f ", fixFPS_->GetDeltaTime());
	ImGui::Text("Frame Count: %d", fixFPS_->GetFrameCount());
	ImGui::DragInt("Target FPS", &fixFPS_->GetTargetFpsPtr(), 1.0f, 12, 60);

	ImGui::End();
}

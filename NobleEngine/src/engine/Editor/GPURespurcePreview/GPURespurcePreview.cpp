#include "GPURespurcePreview.h"
#include <DirectX/DescriptorHeapManager/DescriptorHeapManager.h>
#include <DirectX/DescriptorHeapManager/SRV_UAV/SRV_UAVManager.h>
#include <ImGuiManager/ImGuiManager.h>

GPURespurcePreview::GPURespurcePreview(DescriptorHeapManager* descriptorHeapManager)
	:descriptorHeapManager_(descriptorHeapManager)
{
}

GPURespurcePreview::~GPURespurcePreview()
{
}

void GPURespurcePreview::DrawImGui()
{
	ImGui::Begin("GPU Resources");
	const uint32_t used = descriptorHeapManager_->GetSRV_UAVManager()->GetUsedCount();
	const uint32_t capacity = descriptorHeapManager_->GetSRV_UAVManager()->GetCapacity();
	const float ratio = static_cast<float>(used) / static_cast<float>(capacity);
	ImGui::Text("SRV/UAVスロット : %u / %u", used, capacity);
	ImGui::ProgressBar(ratio, ImVec2(-1.0f, 0.0f));
	ImGui::Text("最大使用数      : %u", descriptorHeapManager_->GetSRV_UAVManager()->GetPeakUsedCount());
	ImGui::End();
}

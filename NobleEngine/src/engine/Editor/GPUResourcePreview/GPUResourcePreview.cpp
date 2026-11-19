#include "GPUResourcePreview.h"
#include <DirectX/DescriptorHeapManager/DescriptorHeapManager.h>
#include <DirectX/DescriptorHeapManager/SRV_UAV/SRV_UAVManager.h>
#include <DirectX/DescriptorHeapManager/DSV/DSVManager.h>
#include <DirectX/DescriptorHeapManager/RTV/RTVManager.h>
#include <ImGuiManager/ImGuiManager.h>

namespace
{
	// ヒープ1枚分の使用状況を表示する
	void DrawHeapUsage(const DescriptorHeap& heap)
	{
		const uint32_t used = heap.GetUsedCount();
		const uint32_t capacity = heap.GetCapacity();
		const float ratio = static_cast<float>(used) / static_cast<float>(capacity);
		ImGui::Text("%sスロット : %u / %u", heap.GetName(), used, capacity);
		ImGui::SameLine();
		ImGui::Text("最大使用数      : %u", heap.GetPeakUsedCount());
		ImGui::ProgressBar(ratio, ImVec2(-1.0f, 0.0f));
	}
}

GPUResourcePreview::GPUResourcePreview(DescriptorHeapManager* descriptorHeapManager)
	:descriptorHeapManager_(descriptorHeapManager)
{}

GPUResourcePreview::~GPUResourcePreview()
{}

void GPUResourcePreview::DrawImGui()
{
	ImGui::Begin("GPU Resources");
	DrawHeapUsage(descriptorHeapManager_->GetSRV_UAVManager()->GetHeap());
	DrawHeapUsage(descriptorHeapManager_->GetDSVManager()->GetHeap());
	DrawHeapUsage(descriptorHeapManager_->GetRTVManager()->GetHeap());
	ImGui::End();
}
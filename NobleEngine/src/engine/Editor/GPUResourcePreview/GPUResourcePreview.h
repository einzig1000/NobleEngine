#pragma once
#include <Editor/IEditorPanel/IEditorPanel.h>


class SRV_UAVManager;

class DescriptorHeapManager;

class GPUResourcePreview : public IEditorPanel
{
public:
	GPUResourcePreview(DescriptorHeapManager* descriptorHeapManager);
	~GPUResourcePreview();

	void DrawImGui() override;

private:
	DescriptorHeapManager* descriptorHeapManager_ = nullptr;

};


#pragma once
#include <Editor/IEditorPanel/IEditorPanel.h>


class SRV_UAVManager;

class DescriptorHeapManager;

class GPURespurcePreview : public IEditorPanel
{
public:
	GPURespurcePreview(DescriptorHeapManager* descriptorHeapManager);
	~GPURespurcePreview();

	void DrawImGui() override;

private:
	DescriptorHeapManager* descriptorHeapManager_ = nullptr;

};


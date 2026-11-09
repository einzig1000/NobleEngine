#pragma once


class SRV_UAVManager;

class DescriptorHeapManager;

class GPURespurcePreview
{
public:
	GPURespurcePreview(DescriptorHeapManager* descriptorHeapManager);
	~GPURespurcePreview();
	void DrawImGui();

private:
	DescriptorHeapManager* descriptorHeapManager_ = nullptr;

};


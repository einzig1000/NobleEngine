#pragma once
#include <d3d12.h>
#include <memory>
#include <DirectX/DescriptorHeapManager/SRV_UAV/SRV_UAVManager.h>
#include <DirectX/DescriptorHeapManager/DSV/DSVManager.h>
#include <DirectX/DescriptorHeapManager/RTV/RTVManager.h>


class DescriptorHeapManager
{
public:
    DescriptorHeapManager(ID3D12Device2* device);
    ~DescriptorHeapManager();

	SRV_UAVManager* GetSRV_UAVManager() const { return srv_uavManager_.get(); }
	RTVManager* GetRTVManager() const { return rtvManager_.get(); }
	DSVManager* GetDSVManager() const { return dsvManager_.get(); }

private:
	std::unique_ptr<SRV_UAVManager> srv_uavManager_;
	std::unique_ptr<RTVManager> rtvManager_;
	std::unique_ptr<DSVManager> dsvManager_;
};
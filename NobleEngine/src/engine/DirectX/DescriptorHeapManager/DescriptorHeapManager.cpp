#include "DescriptorHeapManager.h"
#include <Utilities/Logger/Logger.h>
#include <DirectX/DescriptorHeapManager/SRV_UAV/SRV_UAVManager.h>
#include <DirectX/DescriptorHeapManager/DSV/DSVManager.h>
#include <DirectX/DescriptorHeapManager/RTV/RTVManager.h>


DescriptorHeapManager::DescriptorHeapManager(ID3D12Device2* device)
{
    Logger::Log("コンストラクタ実行開始 : DescriptorHeapManager");

	srv_uavManager_ = std::make_unique<SRV_UAVManager>(device);
	rtvManager_ = std::make_unique<RTVManager>(device);
	dsvManager_ = std::make_unique<DSVManager>(device);

	Logger::Log("成功");
}

DescriptorHeapManager::~DescriptorHeapManager()
{
    Logger::Log("デストラクタ実行成功 : DescriptorHeapManager");
}
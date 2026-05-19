#include "DirectX/DescriptorHeapManager/DescriptorHeapManager.h"
#include "Utilities/Logger/Logger.h"
#include "SRV_UAV/SRV_UAVManager.h"
#include "DSV/DSVManager.h"
#include "RTV/RTVManager.h"


DescriptorHeapManager::DescriptorHeapManager(ID3D12Device* device)
{
	srv_uavManager_ = std::make_unique<SRV_UAVManager>(device);
	rtvManager_ = std::make_unique<RTVManager>(device);
	dsvManager_ = std::make_unique<DSVManager>(device);

    Log("コンストラクタ実行成功 : DescriptorHeapManager");
}

DescriptorHeapManager::~DescriptorHeapManager()
{
    Log("デストラクタ実行成功 : DescriptorHeapManager");
}
#include "RTVManager.h"
#include <Utilities/Logger/Logger.h>

RTVManager::RTVManager(ID3D12Device2* device)
	: device_(device)
	, heap_(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 256, false, "RTV")
{
	Logger::Info("コンストラクタ実行成功 : RTVManager");
}

RTVManager::~RTVManager()
{
	Logger::Info("デストラクタ実行成功 : RTVManager");
}

RTVManager::Allocation RTVManager::CreateRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc)
{
	// 次スロットのインデックス取得
	uint32_t index = heap_.Allocate();

	// ハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap_.GetCPUHandleAt(index);

	// RTV作成
	device_->CreateRenderTargetView(resource, desc, cpuHandle);

	return Allocation{ index, cpuHandle };
}
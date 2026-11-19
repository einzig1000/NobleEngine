#include "DSVManager.h"
#include <Utilities/Logger/Logger.h>

DSVManager::DSVManager(ID3D12Device2* device)
	: device_(device)
	, heap_(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 256, false, "DSV")
{
	Logger::Info("コンストラクタ実行成功 : DSVManager");
}

DSVManager::~DSVManager()
{
	Logger::Info("デストラクタ実行成功 : DSVManager");
}

DSVManager::Allocation DSVManager::CreateDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
{
	// 次スロットのインデックス取得
	uint32_t index = heap_.Allocate();

	// ハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap_.GetCPUHandleAt(index);

	// DSV作成
	device_->CreateDepthStencilView(resource, desc, cpuHandle);

	return Allocation{ index, cpuHandle };
}

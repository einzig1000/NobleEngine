#include "DSVManager.h"
#include <Window/WindowManager.h>
#include "Utilities/Logger/Logger.h"


DSVManager::DSVManager(ID3D12Device* device)
	: device_(device)
{
	// DSVスロット一つ分のサイズを取得
	descriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	// DSV用のディスクリプタヒープ作成
	capacity_ = 10;
	nextIndex_ = 0;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = capacity_;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device_->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap_));
	assert(SUCCEEDED(hr));

	Log("コンストラクタ実行成功 : DescriptorHeapManager");
}

DSVManager::~DSVManager()
{
	Log("デストラクタ実行成功 : DescriptorHeapManager");
}

uint32_t DSVManager::Allocate()
{
	if (nextIndex_ >= capacity_)
	{
		Log("DSVのスロットが足りません。容量を増やしてください。");
		assert(false);
		return UINT32_MAX;
	}
	return nextIndex_++;
}

D3D12_CPU_DESCRIPTOR_HANDLE DSVManager::GetCPUHandleAt(uint32_t index) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (descriptorSize_ * index);
	return handle;
}

DSVManager::Allocation DSVManager::CreateDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
{
	// 次スロットのインデックス取得
	uint32_t index = Allocate();

	// ハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCPUHandleAt(index);

	// DSV作成
	device_->CreateDepthStencilView(resource, desc, cpuHandle);

	return Allocation{ index, cpuHandle };
}

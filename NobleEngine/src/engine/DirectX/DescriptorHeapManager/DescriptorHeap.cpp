#include "DescriptorHeap.h"
#include <Utilities/Logger/Logger.h>
#include <algorithm>
#include <cassert>
#include <cstdlib>

DescriptorHeap::DescriptorHeap(ID3D12Device2* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity, bool shaderVisible, const char* name)
	: name_(name)
	, capacity_(capacity)
	, shaderVisible_(shaderVisible)
{
	// スロット一つ分のサイズを取得
	descriptorSize_ = device->GetDescriptorHandleIncrementSize(type);

	// ディスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.NumDescriptors = capacity_;
	heapDesc.Type = type;
	heapDesc.Flags = shaderVisible_ ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap_));
	assert(SUCCEEDED(hr));

	cpuStart_ = heap_->GetCPUDescriptorHandleForHeapStart();
	// GPUハンドルはシェーダーから見えるヒープにしか無い
	if (shaderVisible_)
	{
		gpuStart_ = heap_->GetGPUDescriptorHandleForHeapStart();
	}

	isFree_.assign(capacity_, false);
}

uint32_t DescriptorHeap::Allocate()
{
	uint32_t index = UINT32_MAX;

	// 空きスロットがあれば再利用
	if (!freeIndices_.empty())
	{
		index = freeIndices_.back();
		freeIndices_.pop_back();
	}
	// 無ければ新しいスロットを使う
	else if (nextIndex_ < capacity_)
	{
		index = nextIndex_++;
	}
	// 容量オーバー
	else
	{
		Logger::Error("%sのスロットが足りません。容量を増やしてください。", name_);
		__debugbreak();
		std::abort();
	}

	isFree_[index] = false;
	peakUsedCount_ = std::max(peakUsedCount_, GetUsedCount());
	return index;
}

void DescriptorHeap::Free(uint32_t index)
{
	if (index == UINT32_MAX) { return; }
	assert(index < nextIndex_);

	if (isFree_[index])
	{
		Logger::Error("%sスロット %u の二重解放を検知しました", name_, index);
		__debugbreak();
		std::abort();
		return;
	}
	isFree_[index] = true;
	freeIndices_.push_back(index);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandleAt(uint32_t index) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = cpuStart_;
	handle.ptr += static_cast<SIZE_T>(descriptorSize_) * index;
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandleAt(uint32_t index) const
{
	assert(shaderVisible_ && "シェーダーから見えないヒープのGPUハンドルは取れません");
	D3D12_GPU_DESCRIPTOR_HANDLE handle = gpuStart_;
	handle.ptr += static_cast<UINT64>(descriptorSize_) * index;
	return handle;
}

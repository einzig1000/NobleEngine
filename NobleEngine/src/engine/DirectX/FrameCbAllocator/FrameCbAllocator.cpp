#include "FrameCbAllocator.h"
#include "DirectX/Resource/Dx12ResourceFactory.h"
#include <cassert>

void FrameCbAllocator::Initialize(ID3D12Device* device, size_t capacityBytes, const wchar_t* debugName)
{
	// デバイスが有効かチェック
	assert(device);

	// 256nに切り上げ
	capacity_ = AlignUp(capacityBytes, 256);

	// リソース作成
	resource_ = Dx12ResourceFactory::CreateBufferResource(device, capacity_);
	assert(resource_);

	// デバッグネーム設定
	resource_->SetName(debugName);

	// リソースをCPUアドレス空間にマップ
	mapped_ = nullptr;
	HRESULT hr = resource_->Map(0, nullptr, reinterpret_cast<void**>(&mapped_));
	assert(SUCCEEDED(hr));
	assert(mapped_);

	offset_ = 0;
}

void FrameCbAllocator::Reset()
{
	offset_ = 0;
}

FrameCbAllocator::Allocation FrameCbAllocator::Allocate(size_t sizeBytes)
{
	assert(mapped_);
	size_t alignedSize = AlignUp(sizeBytes, 256);

	size_t alignedOffset = AlignUp(offset_, 256);
	assert(alignedOffset + alignedSize <= capacity_);
	
	Allocation a{};
	a.cpu = mapped_ + alignedOffset;
	a.gpu = resource_->GetGPUVirtualAddress() + alignedOffset;
	a.size = alignedSize;

	offset_ = alignedOffset + alignedSize;
	return a;
}
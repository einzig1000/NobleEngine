#pragma once
#include <d3d12.h>
#include <cstdint>
#include <DirectX/DescriptorHeapManager/DescriptorHeap.h>

class DSVManager
{
public:
	struct Allocation
	{
		uint32_t index;
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
	};

	DSVManager(ID3D12Device2* device);
	~DSVManager();

	// DSVの作成とスロットの割り当て
	Allocation CreateDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc);

	// スロット管理はヒープに任せる
	uint32_t Allocate() { return heap_.Allocate(); }
	void Free(uint32_t index) { heap_.Free(index); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleAt(uint32_t index) const { return heap_.GetCPUHandleAt(index); }

	// 使用数などの表示用
	const DescriptorHeap& GetHeap() const { return heap_; }

private:
	// デバイス
	ID3D12Device2* device_ = nullptr;
	// DSV用のディスクリプタヒープ
	DescriptorHeap heap_;
};

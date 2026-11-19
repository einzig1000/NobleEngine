#pragma once
#include <d3d12.h>
#include <cstdint>
#include <DirectX/DescriptorHeapManager/DescriptorHeap.h>

class RTVManager
{
public:
	struct Allocation
	{
		uint32_t index;
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
	};

	RTVManager(ID3D12Device2* device);
	~RTVManager();

	// RTVの作成とスロットの割り当て
	Allocation CreateRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc);

	// スロット管理はヒープに任せる
	uint32_t Allocate() { return heap_.Allocate(); }
	void Free(uint32_t index) { heap_.Free(index); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleAt(uint32_t index) const { return heap_.GetCPUHandleAt(index); }

	// 使用数などの表示用
	const DescriptorHeap& GetHeap() const { return heap_; }

private:
	// デバイス
	ID3D12Device2* device_ = nullptr;
	// RTV用のディスクリプタヒープ
	DescriptorHeap heap_;
};

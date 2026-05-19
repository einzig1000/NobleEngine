#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

class DSVManager
{
public:
	struct Allocation
	{
		uint32_t index;
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
	};

	DSVManager(ID3D12Device* device);
	~DSVManager();

	ID3D12DescriptorHeap* GetDSVDescriptorHeap() const { return descriptorHeap_.Get(); }
	uint32_t GetDescriptorSizeDSV() const { return descriptorSize_; }

	// 空いてるスロットインデックスを取得しnextIndex_をインクリメント
	uint32_t Allocate();

	// Allocate()で取得したスロットインデックスのCPUハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleAt(uint32_t index) const;

	// DSVの作成とスロットの割り当て
	Allocation CreateDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc);

private:
	// デバイス
	ID3D12Device* device_ = nullptr;

	// SRV用のディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

	// ディスクリプタサイズ
	uint32_t descriptorSize_;
	// 最大スロット数
	uint32_t capacity_ = 0;
	// 次のスロットインデックス
	uint32_t nextIndex_ = 0;
};


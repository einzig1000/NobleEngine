#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <vector>

/// <summary>
/// ディスクリプタヒープのスロット管理(RTV / DSV / SRV・UAV 共通)
/// ヒープの作成・空きスロットの貸し出しと返却・ハンドル計算・使用数の記録だけを行う
/// ビューを作る処理(CreateRTVなど)は種類ごとに違うので、持ち主のマネージャ側に置く
/// </summary>
class DescriptorHeap
{
public:
	/// <param name="type">ヒープの種類(RTV / DSV / CBV_SRV_UAV)</param>
	/// <param name="capacity">最大スロット数</param>
	/// <param name="shaderVisible">シェーダーから見えるヒープにするか(SRV・UAVだけtrue)</param>
	/// <param name="name">ログ表示用の名前("RTV" など)</param>
	DescriptorHeap(ID3D12Device2* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t capacity, bool shaderVisible, const char* name);

	// 複製の禁止
	DescriptorHeap(const DescriptorHeap&) = delete;
	DescriptorHeap& operator=(const DescriptorHeap&) = delete;

	// 空いてるスロットインデックスを取得して内部情報を更新
	uint32_t Allocate();
	// スロットを空きに戻す。二重解放は無視してログを出す
	void Free(uint32_t index);

	// Allocate()で取得したスロットインデックスのCPUハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleAt(uint32_t index) const;
	// shaderVisible = true のヒープでだけ使える
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleAt(uint32_t index) const;

	ID3D12DescriptorHeap* GetD3D12Heap() const { return heap_.Get(); }
	uint32_t GetDescriptorSize() const { return descriptorSize_; }
	const char* GetName() const { return name_; }

	// 使用中のスロット数
	uint32_t GetUsedCount() const { return nextIndex_ - static_cast<uint32_t>(freeIndices_.size()); }
	// 最大スロット数
	uint32_t GetCapacity() const { return capacity_; }
	// 起動してから一番多く使った時のスロット数
	uint32_t GetPeakUsedCount() const { return peakUsedCount_; }

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_;

	// ログ表示用の名前
	const char* name_ = "";

	// 解放済みで再利用できるスロット
	std::vector<uint32_t> freeIndices_;
	// スロットごとの「空いているか」(二重解放チェック用。1スロット1ビット)
	std::vector<bool> isFree_;

	// 先頭ハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE cpuStart_{};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuStart_{};

	// ディスクリプタサイズ
	uint32_t descriptorSize_ = 0;
	// 最大スロット数
	uint32_t capacity_ = 0;
	// 次のスロットインデックス
	uint32_t nextIndex_ = 0;
	// 使用数の最大値
	uint32_t peakUsedCount_ = 0;
	// シェーダーから見えるヒープか
	bool shaderVisible_ = false;
};
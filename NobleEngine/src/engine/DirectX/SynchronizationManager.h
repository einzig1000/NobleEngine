#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <windows.h>
#include <definition/constexprs.h>

class SynchronizationManager
{
public:
    SynchronizationManager(ID3D12Device* device);
    ~SynchronizationManager();

    void Signal(ID3D12CommandQueue* commandQueue, UINT frameIndex);
	void WaitForGPU(UINT frameIndex);   // フレームごと待ち
    void WaitForGPU();                  // 全体待ち

private:
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceValues[Constexprs::kFrameCount];    // フレームごとのフェンス値
	UINT64 currentFenceValue = 0;       // 全体で増幅されるフェンス値
    HANDLE fenceEvent;
};
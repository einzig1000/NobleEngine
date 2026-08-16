#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <windows.h>
#include <EngineDefinition/EngineConstexprs.h>

class SynchronizationManager
{
public:
    SynchronizationManager(ID3D12Device2* device);
    ~SynchronizationManager();

    void Signal(ID3D12CommandQueue* commandQueue, UINT frameIndex);
	void WaitForGPU(UINT frameIndex);   // フレームごと待ち
    void WaitForGPU();                  // 全体待ち

    // 次にSignalされる予定のフェンス値を取得(まだ発行前の予約値)
    UINT64 GetNextFenceValue() const;
    // 指定した値までフェンスが進んでいるか
    bool IsFenceValueReached(UINT64 value) const;

private:
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceValues[Constexprs::kFrameCount];    // フレームごとのフェンス値
	UINT64 currentFenceValue = 0;       // 全体で増幅されるフェンス値
    HANDLE fenceEvent;
};
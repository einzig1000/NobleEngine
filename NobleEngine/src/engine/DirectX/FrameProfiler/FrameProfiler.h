#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <chrono>
#include <cstdint>
#include <cassert>
#include <EngineDefinition/EngineConstexprs.h>

// フレームごとのCPU/GPU処理時間を計測するクラス。
// GPU計測はID3D12QueryHeap(TIMESTAMP)を使い、WaitForGPUで同期が取れているタイミングで
// kFrameCount分前に書き込んだ結果を読み出す(新たなフェンス待ちを追加しない)。
// CPU計測はstd::chronoで、GPU待ち・FixFPSのFPS制限sleepを含まない
// 「純粋なCPU作業時間(コマンド構築等)」を測る。
class FrameProfiler
{
public:
    FrameProfiler(ID3D12Device2* device, ID3D12CommandQueue* commandQueue);
    ~FrameProfiler();

    // BeginFrame内、WaitForGPU(frameIndex)の直後に呼ぶ
    void OnFrameSynced(UINT frameIndex);

    // BeginFrame内、コマンドリストのリセット直後に呼ぶ(GPU計測区間の開始)
    void BeginGpuScope(ID3D12GraphicsCommandList6* cmdList, UINT frameIndex);
    // EndFrame内、Close()の直前に呼ぶ(GPU計測区間の終了+結果のResolve)
    void EndGpuScope(ID3D12GraphicsCommandList6* cmdList, UINT frameIndex);

    // BeginFrame内、WaitForGPU直後(OnFrameSyncedと同じ位置)に呼ぶ
    void BeginCpuScope();
    // EndFrame内、Close()の直前に呼ぶ
    void EndCpuScope();

    float GetCpuDeltaTimeMs() const { return cpuDeltaTimeMs_; }
    float GetGpuDeltaTimeMs() const { return gpuDeltaTimeMs_; }
    float GetCpuFPS() const { return cpuDeltaTimeMs_ > 0.0f ? 1000.0f / cpuDeltaTimeMs_ : 0.0f; }
    float GetGpuFPS() const { return gpuDeltaTimeMs_ > 0.0f ? 1000.0f / gpuDeltaTimeMs_ : 0.0f; }

private:
    static constexpr uint32_t kQueriesPerFrame = 2; // 開始/終了の2発

    Microsoft::WRL::ComPtr<ID3D12QueryHeap> queryHeap_;
    Microsoft::WRL::ComPtr<ID3D12Resource> readbackBuffer_;
    void* mappedReadback_ = nullptr;

    uint64_t gpuFrequency_ = 1;

    std::chrono::steady_clock::time_point cpuBeginTime_{};
    float cpuDeltaTimeMs_ = 0.0f;
    float gpuDeltaTimeMs_ = 0.0f;
};
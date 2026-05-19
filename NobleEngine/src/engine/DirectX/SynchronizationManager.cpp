#include "DirectX/SynchronizationManager.h"
#include "Utilities/Logger/Logger.h"

SynchronizationManager::SynchronizationManager(ID3D12Device* device)
    : fenceValue(0)
{
    HRESULT hr = device->CreateFence(
        fenceValue,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&fence)
    );
    assert(SUCCEEDED(hr));

    fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent != nullptr);

    Log("コンストラクタ実行成功 : SynchronizationManager");
}

SynchronizationManager::~SynchronizationManager()
{
    if (fenceEvent)
    {
        CloseHandle(fenceEvent);
    }
    Log("デストラクタ実行成功 : SynchronizationManager");
}

void SynchronizationManager::Signal(ID3D12CommandQueue* commandQueue)
{
    fenceValue++;
    HRESULT hr = commandQueue->Signal(fence.Get(), fenceValue);
    assert(SUCCEEDED(hr));
}

void SynchronizationManager::WaitForGPU()
{
    //auto gpuFenceValue = fence->GetCompletedValue();
    //if (gpuFenceValue < fenceValue)
    //{
    //    HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    //    if (eventHandle == nullptr)
    //    {
    //         エラー処理: イベントの作成に失敗した場合
    //        Log("エラー: GPU待機用イベントの作成に失敗しました");
    //        assert(false);
    //        return;
    //    }
    //    HRESULT hr = fence->SetEventOnCompletion(fenceValue, eventHandle);
    //    assert(SUCCEEDED(hr));
    //    WaitForSingleObject(eventHandle, INFINITE);
    //    CloseHandle(eventHandle);
    //}

    const UINT64 gpuFenceValue = fence->GetCompletedValue();
    if (gpuFenceValue < fenceValue)
    {
        HRESULT hr = fence->SetEventOnCompletion(fenceValue, fenceEvent);
        assert(SUCCEEDED(hr));
        WaitForSingleObject(fenceEvent, INFINITE);
    }
}
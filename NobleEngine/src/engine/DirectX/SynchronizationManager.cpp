#include "DirectX/SynchronizationManager.h"
#include "Utilities/Logger/Logger.h"

SynchronizationManager::SynchronizationManager(ID3D12Device* device)
    : currentFenceValue(0)
{
    HRESULT hr = device->CreateFence(
        currentFenceValue,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&fence)
    );
    assert(SUCCEEDED(hr));

    for (UINT i = 0; i < kFrameCount; ++i) fenceValues[i] = 0;

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

void SynchronizationManager::Signal(ID3D12CommandQueue* commandQueue, UINT frameIndex)
{
    currentFenceValue++;
	fenceValues[frameIndex] = currentFenceValue;
    HRESULT hr = commandQueue->Signal(fence.Get(), fenceValues[frameIndex]);
    assert(SUCCEEDED(hr));
}

void SynchronizationManager::WaitForGPU(UINT frameIndex)
{
	const UINT64 completedValue = fence->GetCompletedValue();
	if (completedValue < fenceValues[frameIndex])
	{
		HRESULT hr = fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
		assert(SUCCEEDED(hr));
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void SynchronizationManager::WaitForGPU()
{
    const UINT64 completedValue = fence->GetCompletedValue();
    if (completedValue < currentFenceValue)
    {
        HRESULT hr = fence->SetEventOnCompletion(currentFenceValue, fenceEvent);
        assert(SUCCEEDED(hr));
        WaitForSingleObject(fenceEvent, INFINITE);
    }
}
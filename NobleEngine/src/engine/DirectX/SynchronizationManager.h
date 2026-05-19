#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <windows.h>

class SynchronizationManager
{
public:
    SynchronizationManager(ID3D12Device* device);
    ~SynchronizationManager();

    void Signal(ID3D12CommandQueue* commandQueue);
    void WaitForGPU();

private:
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    UINT64 fenceValue;
    HANDLE fenceEvent;
};
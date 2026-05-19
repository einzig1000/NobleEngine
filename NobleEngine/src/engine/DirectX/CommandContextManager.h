#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>

class CommandContextManager
{
public:
    CommandContextManager(ID3D12Device* device);
    ~CommandContextManager();

    ID3D12CommandQueue* GetCommandQueue() const { return commandQueue.Get(); }
    ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
    ID3D12CommandAllocator* GetCommandAllocator() const { return commandAllocator.Get(); }

    void ResetCommandList();

private:
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
};
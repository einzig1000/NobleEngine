#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <definition/constexprs.h>

class CommandContextManager
{
public:
    CommandContextManager(ID3D12Device* device);
    ~CommandContextManager();

    ID3D12CommandQueue* GetCommandQueue() const { return commandQueue.Get(); }
    ID3D12GraphicsCommandList* GetCommandList(UINT frameIndex) const { return commandList[frameIndex].Get(); }
    ID3D12CommandAllocator* GetCommandAllocator(UINT frameIndex) const { return commandAllocators[frameIndex].Get(); }

    void ResetCommandList(UINT frameIndex);

private:
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList[kFrameCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[kFrameCount];
};
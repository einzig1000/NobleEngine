#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <EngineDefinition/EngineConstexprs.h>

class CommandContextManager
{
public:
    CommandContextManager(ID3D12Device2* device);
    ~CommandContextManager();

    ID3D12CommandQueue* GetCommandQueue() const { return commandQueue.Get(); }
    ID3D12GraphicsCommandList6* GetCommandList(UINT frameIndex) const { return commandList[frameIndex].Get(); }
    ID3D12CommandAllocator* GetCommandAllocator(UINT frameIndex) const { return commandAllocators[frameIndex].Get(); }

    void ResetCommandList(UINT frameIndex);

private:
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> commandList[Constexprs::kFrameCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[Constexprs::kFrameCount];
};
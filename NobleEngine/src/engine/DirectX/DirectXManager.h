#pragma once
#include <d3d12.h>
#include <memory>

#include <DirectX/DeviceManager/DeviceManager.h>
#include <DirectX/CommandContextManager/CommandContextManager.h>
#include <DirectX/RenderTarget/SwapChainManager/SwapChainManager.h>
#include <DirectX/RenderTarget/RenderTextureManager/RenderTextureManager.h>
#include <DirectX/Pipeline/PipelineStateManager/PipelineStateManager.h>
#include <DirectX/DescriptorHeapManager/DescriptorHeapManager.h>
#include <DirectX/SynchronizationManager/SynchronizationManager.h>
#include <DirectX/FrameProfiler/FrameProfiler.h>

/// <summary>
/// DirectX管理クラス
/// </summary>
class DirectXManager
{
public:
    DirectXManager(HWND hwnd);
    ~DirectXManager();

    ID3D12Device2* GetDevice() const { return deviceManager->GetDevice(); }
    CommandContextManager* GetCommandContextManager() const { return commandContextManager.get(); }
    DescriptorHeapManager* GetDescriptorHeapManager() const { return descriptorHeapManager.get(); }
    SwapChainManager* GetSwapChain() const { return swapChainManager.get(); };
    PipelineStateManager* GetPipelineStateManager() const { return pipelineStateManager.get(); }
    SynchronizationManager* GetSynchronizationManager() const { return synchronizationManager.get(); }
    RenderTextureManager* GetRenderTextureManager() const { return renderTextureManager.get(); }
    FrameProfiler* GetFrameProfiler() const { return frameProfiler.get(); }

    // フレーム開始処理
    void BeginFrame();
    // フレーム終了処理
    void EndFrame();

    // 書き込みたいRenderTextureを指定してResourceのStateをD3D12_RESOURCE_STATE_RENDER_TARGETに遷移させる。
    void BeginRenderPass(RenderTarget* target, bool isDepthWrite);
    // SetRenderTargetで変えたResouruceのStateをD3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCEに遷移させる。
	void EndRenderPass(RenderTarget* target, bool isDepthWrite, D3D12_RESOURCE_STATES nextState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    void Resize();

private:
    std::unique_ptr<SwapChainManager> swapChainManager;
    std::unique_ptr<DeviceManager> deviceManager;
    std::unique_ptr<CommandContextManager> commandContextManager;
    std::unique_ptr<PipelineStateManager> pipelineStateManager;
    std::unique_ptr<DescriptorHeapManager> descriptorHeapManager;
    std::unique_ptr<SynchronizationManager> synchronizationManager;
	std::unique_ptr<RenderTextureManager> renderTextureManager;
    std::unique_ptr<FrameProfiler> frameProfiler;
};
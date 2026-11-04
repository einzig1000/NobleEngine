#pragma once
#include <d3d12.h>
#include <memory>
#include <DirectX/RenderTarget/RenderTargetStruct.h>

class SwapChainManager;
class DeviceManager;
class CommandContextManager;
class PipelineStateManager;
class DescriptorHeapManager;
class SynchronizationManager;
class RenderTextureManager;
class RootBindingManager;
class FrameProfiler;

/// <summary>
/// DirectX管理クラス
/// </summary>
class DirectXManager
{
public:
    DirectXManager(HWND hwnd);
    ~DirectXManager();

    ID3D12Device2* GetDevice() const;
    CommandContextManager* GetCommandContextManager() const { return commandContextManager_.get(); }
    DescriptorHeapManager* GetDescriptorHeapManager() const { return descriptorHeapManager_.get(); }
    SwapChainManager* GetSwapChainManager() const { return swapChainManager_.get(); };
    PipelineStateManager* GetPipelineStateManager() const { return pipelineStateManager_.get(); }
    SynchronizationManager* GetSynchronizationManager() const { return synchronizationManager_.get(); }
    RenderTextureManager* GetRenderTextureManager() const { return renderTextureManager_.get(); }
    RootBindingManager* GetRootBindingManager() { return rootBindingManager_.get(); }
    FrameProfiler* GetFrameProfiler() const { return frameProfiler_.get(); }

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
    std::unique_ptr<SwapChainManager> swapChainManager_;
    std::unique_ptr<DeviceManager> deviceManager_;
    std::unique_ptr<CommandContextManager> commandContextManager_;
    std::unique_ptr<PipelineStateManager> pipelineStateManager_;
    std::unique_ptr<DescriptorHeapManager> descriptorHeapManager_;
    std::unique_ptr<SynchronizationManager> synchronizationManager_;
    std::unique_ptr<RenderTextureManager> renderTextureManager_;
    std::unique_ptr<RootBindingManager> rootBindingManager_;
    std::unique_ptr<FrameProfiler> frameProfiler_;
};

#pragma once
#include <d3d12.h>
#include <memory>

#include <DirectX/DeviceManager.h>
#include <DirectX/CommandContextManager.h>
#include <DirectX/SwapChainManager.h>
#include <DirectX/Pipeline/PipelineStateManager/PipelineStateManager.h>
#include <DirectX/DescriptorHeapManager/DescriptorHeapManager.h>
#include <DirectX/SynchronizationManager.h>
#include <DirectX/ViewportScissorManager.h>
#include <DirectX/RenderTextureManager/RenderTextureManager.h>

/// <summary>
/// DirectX管理クラス
/// </summary>
class DirectXManager
{
public:
    DirectXManager(HWND hwnd);
    ~DirectXManager();

    ID3D12Device* GetDevice() const { return deviceManager->GetDevice(); }
    CommandContextManager* GetCommandContextManager() const { return commandContextManager.get(); }
    DescriptorHeapManager* GetDescriptorHeapManager() const { return descriptorHeapManager.get(); }
    SwapChainManager* GetSwapChain() const { return swapChainManager.get(); };
    PipelineStateManager* GetPipelineStateManager() const { return pipelineStateManager.get(); }
    SynchronizationManager* GetSynchronizationManager() const { return synchronizationManager.get(); }
    RenderTextureManager* GetRenderTextureManager() const { return renderTextureManager.get(); }
    // フレーム開始処理
    void BeginFrame();
	// オブジェクト描画前に呼び出す。
	void PreSceneDraw();
	// オブジェクト描画後に呼び出す。
	void PostSceneDraw();
	// スクリーン描画前に呼び出す。
	void PreScreenDraw();
	// スクリーン描画後に呼び出す。
	void PostScreenDraw();
    // フレーム終了処理
    void EndFrame();
    void Resize();

private:
    std::unique_ptr<SwapChainManager> swapChainManager;
    std::unique_ptr<DeviceManager> deviceManager;
    std::unique_ptr<CommandContextManager> commandContextManager;
    std::unique_ptr<PipelineStateManager> pipelineStateManager;
    std::unique_ptr<DescriptorHeapManager> descriptorHeapManager;
    std::unique_ptr<SynchronizationManager> synchronizationManager;
    std::unique_ptr<ViewportScissorManager> viewportScissorManager;
	std::unique_ptr<RenderTextureManager> renderTextureManager;
};
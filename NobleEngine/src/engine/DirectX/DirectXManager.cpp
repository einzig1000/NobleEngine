#include "DirectXManager.h"
#include <Utilities/Logger/Logger.h>
#include <DirectX/ResourceUtilities/ResourceUtilities.h>

namespace
{
    void Transition(ID3D12GraphicsCommandList6* cmd, RenderTarget* target, bool isDepth, D3D12_RESOURCE_STATES afterState)
    {
		ID3D12Resource* resource = isDepth ? target->depthResource.Get() : target->colorResource.Get();
		D3D12_RESOURCE_STATES& beforeState = isDepth ? target->dsvState : target->state;

		if (beforeState == afterState) return;

        Dx12ResourceTransition::Transition(cmd, resource, beforeState, afterState);
        
        beforeState = afterState;
    }
};

DirectXManager::DirectXManager(HWND hwnd)
{
    Log("コンストラクタ実行開始 : DirectXManager");

    deviceManager = std::make_unique<DeviceManager>();
    commandContextManager = std::make_unique<CommandContextManager>(deviceManager->GetDevice());
    descriptorHeapManager = std::make_unique<DescriptorHeapManager>(deviceManager->GetDevice());
    pipelineStateManager = std::make_unique<PipelineStateManager>(deviceManager->GetDevice());
    swapChainManager = std::make_unique<SwapChainManager>(deviceManager->GetDevice(), commandContextManager->GetCommandQueue(), hwnd, descriptorHeapManager.get());
	renderTextureManager = std::make_unique<RenderTextureManager>(deviceManager->GetDevice(), commandContextManager->GetCommandQueue(), descriptorHeapManager.get());
    synchronizationManager = std::make_unique<SynchronizationManager>(deviceManager->GetDevice());
    frameProfiler = std::make_unique<FrameProfiler>(deviceManager->GetDevice(), commandContextManager->GetCommandQueue());

    Log("成功");
}

DirectXManager::~DirectXManager()
{
    Log("デストラクタ実行成功 : DirectXManager");
}

void DirectXManager::BeginFrame()
{
    // バックバッファのインデックスを更新
    swapChainManager->UpdateBackBufferIndex();
    UINT backBufferIndex = swapChainManager->GetCurrentBackBufferIndex();

    // フレーム単位の GPU 完了待ち（このフレームで使う CommandAllocator が GPU によってまだ使われている場合は待つ）
    synchronizationManager->WaitForGPU(backBufferIndex);

    // GPU計測結果の読み出し(2フレーム前の分) + CPU計測開始
    frameProfiler->OnFrameSynced(backBufferIndex);
    frameProfiler->BeginCpuScope();

    // コマンドリストをリセット
    commandContextManager->ResetCommandList(backBufferIndex);

    // GPU計測区間の開始
    frameProfiler->BeginGpuScope(commandContextManager->GetCommandList(backBufferIndex), backBufferIndex);

	// SRV/UAV用のDescriptorHeapをセット
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeapManager->GetSRV_UAVManager()->GetSRVDescriptorHeap() };
    commandContextManager->GetCommandList(backBufferIndex)->SetDescriptorHeaps(1, descriptorHeaps);
}

void DirectXManager::BeginRenderPass(RenderTarget* target, bool isDepthWrite)
{
	// バックバッファのインデックスを取得
    UINT backBufferIndex = swapChainManager->GetCurrentBackBufferIndex();

    // リソースの状態をRenderTargetに遷移
    Transition(commandContextManager->GetCommandList(backBufferIndex), target, false, D3D12_RESOURCE_STATE_RENDER_TARGET);
    if (isDepthWrite)
    {
        Transition(commandContextManager->GetCommandList(backBufferIndex), target, true, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    }

    // 描画先のRTVとDSVを指定
    if (isDepthWrite)
    {
        commandContextManager->GetCommandList(backBufferIndex)->OMSetRenderTargets(1, &target->rtvAlloc.handle, false, &target->dsvAlloc.handle);
    }
    else
    {
        commandContextManager->GetCommandList(backBufferIndex)->OMSetRenderTargets(1, &target->rtvAlloc.handle, false, nullptr);
    }

    // RTVのクリア
    commandContextManager->GetCommandList(backBufferIndex)->ClearRenderTargetView(target->rtvAlloc.handle, target->clearColor, 0, nullptr);
    // DSVのクリア
    if (isDepthWrite)
    {
        commandContextManager->GetCommandList(backBufferIndex)->ClearDepthStencilView(target->dsvAlloc.handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }

    // ViewportとScissorを設定
    commandContextManager->GetCommandList(backBufferIndex)->RSSetViewports(1, &target->viewport);
    commandContextManager->GetCommandList(backBufferIndex)->RSSetScissorRects(1, &target->scissorRect);
}

void DirectXManager::EndRenderPass(RenderTarget* target, bool isDepthWrite, D3D12_RESOURCE_STATES nextState)
{
    // バックバッファのインデックスを取得
    UINT backBufferIndex = swapChainManager->GetCurrentBackBufferIndex();

	// リソースの状態をPixelShaderResourceに遷移
	Transition(commandContextManager->GetCommandList(backBufferIndex), target, false, nextState);
	if (isDepthWrite)
	{
		Transition(commandContextManager->GetCommandList(backBufferIndex), target, true, nextState);
	}
}

void DirectXManager::EndFrame()
{
    // バックバッファのインデックスを取得
    UINT backBufferIndex = swapChainManager->GetCurrentBackBufferIndex();

    // GPU計測区間の終了 + CPU計測終了
    frameProfiler->EndGpuScope(commandContextManager->GetCommandList(backBufferIndex), backBufferIndex);
    frameProfiler->EndCpuScope();

    // コマンドリストを確定・実行
    HRESULT hr = commandContextManager->GetCommandList(backBufferIndex)->Close();
    if (FAILED(hr))
    {
        Log("コマンドリストの確定に失敗 %s", HrToString(hr));
        assert(false);
    }
    ID3D12CommandList* commandLists[] = { commandContextManager->GetCommandList(backBufferIndex) };
    commandContextManager->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

    // スワップチェーンをプレゼンテーション
    swapChainManager->Present();

    // フェンスシグナル
    synchronizationManager->Signal(commandContextManager->GetCommandQueue(), backBufferIndex);
}

void DirectXManager::Resize()
{
	// スワップチェーンのリサイズ
	swapChainManager->Resize(
		GetDevice(),
		GetCommandContextManager()->GetCommandQueue());
}
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

    deviceManager_ = std::make_unique<DeviceManager>();
    commandContextManager_ = std::make_unique<CommandContextManager>(deviceManager_->GetDevice());
    descriptorHeapManager_ = std::make_unique<DescriptorHeapManager>(deviceManager_->GetDevice());
    pipelineStateManager_ = std::make_unique<PipelineStateManager>(deviceManager_->GetDevice());
    swapChainManager_ = std::make_unique<SwapChainManager>(deviceManager_->GetDevice(), commandContextManager_->GetCommandQueue(), hwnd, descriptorHeapManager_.get());
	renderTextureManager_ = std::make_unique<RenderTextureManager>(deviceManager_->GetDevice(), commandContextManager_->GetCommandQueue(), descriptorHeapManager_.get());
    synchronizationManager_ = std::make_unique<SynchronizationManager>(deviceManager_->GetDevice());
    frameProfiler_ = std::make_unique<FrameProfiler>(deviceManager_->GetDevice(), commandContextManager_->GetCommandQueue());

    Log("成功");
}

DirectXManager::~DirectXManager()
{
    Log("デストラクタ実行成功 : DirectXManager");
}

void DirectXManager::BeginFrame()
{
    // バックバッファのインデックスを更新
    swapChainManager_->UpdateBackBufferIndex();
    UINT backBufferIndex = swapChainManager_->GetCurrentBackBufferIndex();

    // フレーム単位の GPU 完了待ち（このフレームで使う CommandAllocator が GPU によってまだ使われている場合は待つ）
    synchronizationManager_->WaitForGPU(backBufferIndex);

    // GPU計測結果の読み出し(2フレーム前の分) + CPU計測開始
    frameProfiler_->OnFrameSynced(backBufferIndex);
    frameProfiler_->BeginCpuScope();

    // コマンドリストをリセット
    commandContextManager_->ResetCommandList(backBufferIndex);

    // GPU計測区間の開始
    frameProfiler_->BeginGpuScope(commandContextManager_->GetCommandList(backBufferIndex), backBufferIndex);

	// SRV/UAV用のDescriptorHeapをセット
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeapManager_->GetSRV_UAVManager()->GetSRVDescriptorHeap() };
    commandContextManager_->GetCommandList(backBufferIndex)->SetDescriptorHeaps(1, descriptorHeaps);
}

void DirectXManager::BeginRenderPass(RenderTarget* target, bool isDepthWrite)
{
	// バックバッファのインデックスを取得
    UINT backBufferIndex = swapChainManager_->GetCurrentBackBufferIndex();

    // リソースの状態をRenderTargetに遷移
    Transition(commandContextManager_->GetCommandList(backBufferIndex), target, false, D3D12_RESOURCE_STATE_RENDER_TARGET);
    if (isDepthWrite)
    {
        Transition(commandContextManager_->GetCommandList(backBufferIndex), target, true, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    }

    // 描画先のRTVとDSVを指定
    if (isDepthWrite)
    {
        commandContextManager_->GetCommandList(backBufferIndex)->OMSetRenderTargets(1, &target->rtvAlloc.handle, false, &target->dsvAlloc.handle);
    }
    else
    {
        commandContextManager_->GetCommandList(backBufferIndex)->OMSetRenderTargets(1, &target->rtvAlloc.handle, false, nullptr);
    }

    // RTVのクリア
    commandContextManager_->GetCommandList(backBufferIndex)->ClearRenderTargetView(target->rtvAlloc.handle, target->clearColor, 0, nullptr);
    // DSVのクリア
    if (isDepthWrite)
    {
        commandContextManager_->GetCommandList(backBufferIndex)->ClearDepthStencilView(target->dsvAlloc.handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }

    // ViewportとScissorを設定
    commandContextManager_->GetCommandList(backBufferIndex)->RSSetViewports(1, &target->viewport);
    commandContextManager_->GetCommandList(backBufferIndex)->RSSetScissorRects(1, &target->scissorRect);
}

void DirectXManager::EndRenderPass(RenderTarget* target, bool isDepthWrite, D3D12_RESOURCE_STATES nextState)
{
    // バックバッファのインデックスを取得
    UINT backBufferIndex = swapChainManager_->GetCurrentBackBufferIndex();

	// リソースの状態をPixelShaderResourceに遷移
	Transition(commandContextManager_->GetCommandList(backBufferIndex), target, false, nextState);
	if (isDepthWrite)
	{
		Transition(commandContextManager_->GetCommandList(backBufferIndex), target, true, nextState);
	}
}

void DirectXManager::EndFrame()
{
    // バックバッファのインデックスを取得
    UINT backBufferIndex = swapChainManager_->GetCurrentBackBufferIndex();

    // GPU計測区間の終了 + CPU計測終了
    frameProfiler_->EndGpuScope(commandContextManager_->GetCommandList(backBufferIndex), backBufferIndex);
    frameProfiler_->EndCpuScope();

    // コマンドリストを確定・実行
    HRESULT hr = commandContextManager_->GetCommandList(backBufferIndex)->Close();
    if (FAILED(hr))
    {
        Log("コマンドリストの確定に失敗 %s", HrToString(hr));
        assert(false);
    }
    ID3D12CommandList* commandLists[] = { commandContextManager_->GetCommandList(backBufferIndex) };
    commandContextManager_->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

    // スワップチェーンをプレゼンテーション
    swapChainManager_->Present();

    // フェンスシグナル
    synchronizationManager_->Signal(commandContextManager_->GetCommandQueue(), backBufferIndex);
}

void DirectXManager::Resize()
{
	// スワップチェーンのリサイズ
	swapChainManager_->Resize(
		GetDevice(),
		GetCommandContextManager()->GetCommandQueue());
}

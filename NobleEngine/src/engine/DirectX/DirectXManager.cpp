#include "DirectX/DirectXManager.h"
#include "Utilities/Logger/Logger.h"

namespace
{
    void Transition(ID3D12GraphicsCommandList* cmd, RenderTarget* target, bool isDepth, D3D12_RESOURCE_STATES newState)
    {
		ID3D12Resource* resource = isDepth ? target->dsvResource.Get() : target->resource.Get();
		D3D12_RESOURCE_STATES& pre = isDepth ? target->dsvState : target->state;

		if (pre == newState) return;

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = resource;
        barrier.Transition.StateBefore = pre;
        barrier.Transition.StateAfter = newState;
        cmd->ResourceBarrier(1, &barrier);
        
		pre = newState;
    }
};

DirectXManager::DirectXManager(HWND hwnd)
{
    deviceManager = std::make_unique<DeviceManager>();
    commandContextManager = std::make_unique<CommandContextManager>(deviceManager->GetDevice());
    descriptorHeapManager = std::make_unique<DescriptorHeapManager>(deviceManager->GetDevice());
    swapChainManager = std::make_unique<SwapChainManager>(deviceManager->GetDevice(), commandContextManager->GetCommandQueue(), hwnd, descriptorHeapManager.get());
    pipelineStateManager = std::make_unique<PipelineStateManager>(deviceManager->GetDevice());
	renderTextureManager = std::make_unique<RenderTextureManager>(deviceManager->GetDevice(), descriptorHeapManager.get());
    synchronizationManager = std::make_unique<SynchronizationManager>(deviceManager->GetDevice());
    viewportScissorManager = std::make_unique<ViewportScissorManager>();

    Log("コンストラクタ実行成功 : DirectXManager");
}

DirectXManager::~DirectXManager()
{
    Log("デストラクタ実行成功 : DirectXManager");
}

void DirectXManager::BeginFrame()
{
    // コマンドリストをリセット
    commandContextManager->ResetCommandList();

    // バックバッファのインデックスを更新
    swapChainManager->UpdateBackBufferIndex();
    UINT backBufferIndex = swapChainManager->GetCurrentBackBufferIndex();

	// SRV/UAV用のDescriptorHeapをセット
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeapManager->GetSRV_UAVManager()->GetSRVDescriptorHeap() };
    commandContextManager->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void DirectXManager::BeginRenderPass(RenderTarget* target, bool isDepthWrite)
{
    // リソースの状態をRenderTargetに遷移
    Transition(commandContextManager->GetCommandList(), target, false, D3D12_RESOURCE_STATE_RENDER_TARGET);
    if (isDepthWrite)
    {
        Transition(commandContextManager->GetCommandList(), target, true, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    }

    // 描画先のRTVとDSVを指定
    if (isDepthWrite)
    {
        commandContextManager->GetCommandList()->OMSetRenderTargets(1, &target->rtvAlloc.handle, false, &target->dsvAlloc.handle);
    }
    else
    {
        commandContextManager->GetCommandList()->OMSetRenderTargets(1, &target->rtvAlloc.handle, false, nullptr);
    }

    // RTVのクリア
    float clearColor[] = { 0.396078f, 0.894117f, 1.0f, 1.0f };
    commandContextManager->GetCommandList()->ClearRenderTargetView(target->rtvAlloc.handle, clearColor, 0, nullptr);
    // DSVのクリア
    if (isDepthWrite)
    {
        commandContextManager->GetCommandList()->ClearDepthStencilView(target->dsvAlloc.handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }

    // ViewportとScissorを設定
    commandContextManager->GetCommandList()->RSSetViewports(1, &target->viewport);
    commandContextManager->GetCommandList()->RSSetScissorRects(1, &target->scissorRect);
}

void DirectXManager::EndRenderPass(RenderTarget* target, bool isDepthWrite, D3D12_RESOURCE_STATES nextState)
{
	// リソースの状態をPixelShaderResourceに遷移
	Transition(commandContextManager->GetCommandList(), target, false, nextState);
	if (isDepthWrite)
	{
		Transition(commandContextManager->GetCommandList(), target, true, nextState);
	}
}

void DirectXManager::EndFrame()
{
    // コマンドリストを確定・実行
    HRESULT hr = commandContextManager->GetCommandList()->Close();
    if (FAILED(hr))
    {
        Log("コマンドリストの確定に失敗 %s", HrToString(hr));
        assert(false);
    }
    ID3D12CommandList* commandLists[] = { commandContextManager->GetCommandList() };
    commandContextManager->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

    // スワップチェーンをプレゼンテーション
    swapChainManager->Present();

    // フェンスシグナル
    synchronizationManager->Signal(commandContextManager->GetCommandQueue());
}

void DirectXManager::Resize()
{
	// スワップチェーンのリサイズ
	swapChainManager->Resize(
		GetDevice(),
		GetCommandContextManager()->GetCommandQueue());

	// ビューポートとシザー矩形の更新
	viewportScissorManager->Resize();
}
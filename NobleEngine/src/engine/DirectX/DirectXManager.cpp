#include "DirectX/DirectXManager.h"
#include "Utilities/Logger/Logger.h"

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
}

void DirectXManager::PreSceneDraw()
{
    // オフスクリーンレンダリング用のRenderTextureを取得
    RenderTexture* offscreenRenderTarget = renderTextureManager->Get("RenderTarget_0");

    // オフスクリーンをRenderTargetにセット
    D3D12_RESOURCE_BARRIER barriers[2] = {};
    barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barriers[0].Transition.pResource = offscreenRenderTarget->resource.Get();
    barriers[0].Transition.StateBefore = offscreenRenderTarget->currentState;
    barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    offscreenRenderTarget->currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;

    barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barriers[1].Transition.pResource = offscreenRenderTarget->dsvResource.Get();
    barriers[1].Transition.StateBefore = offscreenRenderTarget->dsvCurrentState;
    barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    offscreenRenderTarget->dsvCurrentState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

    // rtvResourceとdsvResourceのResourceStateを遷移
    commandContextManager->GetCommandList()->ResourceBarrier(2, barriers);

    // srvResourceのResourceStateをGENERIC_READに遷移
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeapManager->GetSRV_UAVManager()->GetSRVDescriptorHeap() };
    commandContextManager->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

    // 描画先のRTVとDSVを指定
    D3D12_CPU_DESCRIPTOR_HANDLE offscreenRTVHandle = offscreenRenderTarget->rtvAlloc.handle;
    D3D12_CPU_DESCRIPTOR_HANDLE offscreenDSVHandle = offscreenRenderTarget->dsvAlloc.handle;
    commandContextManager->GetCommandList()->OMSetRenderTargets(1, &offscreenRTVHandle, false, &offscreenDSVHandle);

    // クリア
    float clearColor[] = { 0.396078f, 0.894117f, 1.0f, 1.0f };
    commandContextManager->GetCommandList()->ClearRenderTargetView(offscreenRTVHandle, clearColor, 0, nullptr);
    commandContextManager->GetCommandList()->ClearDepthStencilView(offscreenDSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // ViewportとScissorを設定
    commandContextManager->GetCommandList()->RSSetViewports(1, &offscreenRenderTarget->viewport);
    commandContextManager->GetCommandList()->RSSetScissorRects(1, &offscreenRenderTarget->scissorRect);
}

void DirectXManager::PostSceneDraw()
{
    // オフスクリーンレンダリング用のRenderTextureを取得
    RenderTexture* offscreenRenderTarget = renderTextureManager->Get("RenderTarget_0");

    // オフスクリーンをRenderTargetにセット
    D3D12_RESOURCE_BARRIER barriers[2] = {};
    barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barriers[0].Transition.pResource = offscreenRenderTarget->resource.Get();
    barriers[0].Transition.StateBefore = offscreenRenderTarget->currentState;
	barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	offscreenRenderTarget->currentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[1].Transition.pResource = offscreenRenderTarget->dsvResource.Get();
	barriers[1].Transition.StateBefore = offscreenRenderTarget->dsvCurrentState;
	barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	offscreenRenderTarget->dsvCurrentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	// rtvResourceとdsvResourceのResourceStateを遷移
	commandContextManager->GetCommandList()->ResourceBarrier(2, barriers);

}

void DirectXManager::PreScreenDraw()
{
    // swapChainのBackBufferResourceのResourceStateをPRESENTからRENDER_TARGETへ遷移
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = swapChainManager->GetCurrentBackBufferResource();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    commandContextManager->GetCommandList()->ResourceBarrier(1, &barrier);

    // srvResourceのResourceStateをGENERIC_READに遷移
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeapManager->GetSRV_UAVManager()->GetSRVDescriptorHeap() };
    commandContextManager->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

    // 描画先のRTVとDSVを指定
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChainManager->GetCurrentRTVHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = swapChainManager->GetDSVHandle();
    //commandContextManager->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
    commandContextManager->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

    // クリア
    float clearColor[] = { 0.396078f, 0.894117f, 1.0f, 1.0f };
    commandContextManager->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // ViewportとScissorを設定
    commandContextManager->GetCommandList()->RSSetViewports(1, &viewportScissorManager->GetViewport());
    commandContextManager->GetCommandList()->RSSetScissorRects(1, &viewportScissorManager->GetScissorRect());
}

void DirectXManager::PostScreenDraw()
{
    // swapChainのBackBufferResourceのResourceStateをRENDER_TARGETからPRESENTへ遷移
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = swapChainManager->GetCurrentBackBufferResource();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandContextManager->GetCommandList()->ResourceBarrier(1, &barrier);
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
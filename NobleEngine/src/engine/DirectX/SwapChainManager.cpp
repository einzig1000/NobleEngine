#include <DirectX/SwapChainManager.h>
#include <Utilities/Logger/Logger.h>
#include <Window/WindowManager.h>
#include <DirectX/Resource/Dx12ResourceFactory.h>
#include <Windows.h> 
#include <cassert>

SwapChainManager::SwapChainManager(ID3D12Device* device, ID3D12CommandQueue* commandQueue, HWND hwnd, DescriptorHeapManager* descriptorHeapManager)
	: device_(device), descriptorHeapManager_(descriptorHeapManager)
{
	InitializeSwapChainInternal(device, commandQueue, hwnd);
	InitializeRenderTargetView(device);
	InitializeDepthStencilView(device);
	backBufferIndex_ = 0;

	Log("コンストラクタ実行成功 : SwapChainManager");
}

SwapChainManager::~SwapChainManager()
{
	Log("デストラクタ実行成功 : SwapChainManager");
}

void SwapChainManager::InitializeSwapChainInternal(ID3D12Device* device, ID3D12CommandQueue* commandQueue, HWND hwnd)
{
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    assert(SUCCEEDED(hr));

    swapChainDesc_.Width = UINT(WindowManager::winWidth_);
    swapChainDesc_.Height = UINT(WindowManager::winHeight_);
    swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    swapChainDesc_.SampleDesc.Count = 1;
    swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc_.BufferCount = 2;
    swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
    hr = dxgiFactory->CreateSwapChainForHwnd(
        commandQueue, hwnd, &swapChainDesc_, nullptr, nullptr,
        swapChain1.ReleaseAndGetAddressOf());
    assert(SUCCEEDED(hr));

    // IDXGISwapChain4 へ昇格
    hr = swapChain1.As(&swapChain_);
    assert(SUCCEEDED(hr));

    hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
    assert(SUCCEEDED(hr));
    hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
    assert(SUCCEEDED(hr));
}

void SwapChainManager::InitializeRenderTargetView(ID3D12Device* device)
{
	rtvDesc_ = {};
    rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    for (int i = 0; i < 2; ++i)
    {
        if (rtvAllocations_[i].index == UINT32_MAX)
        {
			rtvAllocations_[i] = descriptorHeapManager_->GetRTVManager()->CreateRTV(swapChainResources_[i].Get(), &rtvDesc_);
        }
    }
}

void SwapChainManager::InitializeDepthStencilView(ID3D12Device* device)
{
    depthStencilResource_ = Dx12ResourceFactory::CreateDepthStencilResource(device, UINT(WindowManager::winWidth_), UINT(WindowManager::winHeight_));

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

    if (mainDepthDSV_.index == UINT32_MAX)
    {
		mainDepthDSV_ = descriptorHeapManager_->GetDSVManager()->CreateDSV(depthStencilResource_.Get(), &dsvDesc);
    }
}

void SwapChainManager::Present()
{
	HRESULT hr = swapChain_->Present(1, 0);
    // FAILED(hr) の場合に device->GetDeviceRemovedReason() をログ出し
    if (FAILED(hr))
    {
        const HRESULT removed = device_->GetDeviceRemovedReason();

        Log("SwapChainManager::Present failed. hr=0x%08X, GetDeviceRemovedReason=0x%08X",
            static_cast<unsigned int>(hr),
            static_cast<unsigned int>(removed));

	}
}

void SwapChainManager::UpdateBackBufferIndex()
{
    backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
}

void SwapChainManager::Resize(ID3D12Device* device, ID3D12CommandQueue* commandQueue)
{
    if (WindowManager::winWidth_ == 0 || WindowManager::winHeight_ == 0) return;

    // 全GPU作業の完了待ち（厳密）
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    assert(SUCCEEDED(hr));
    HANDLE evt = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    assert(evt != nullptr);
    UINT64 value = 1;
    hr = commandQueue->Signal(fence.Get(), value);
    assert(SUCCEEDED(hr));
    hr = fence->SetEventOnCompletion(value, evt);
    assert(SUCCEEDED(hr));
    WaitForSingleObject(evt, INFINITE);
    CloseHandle(evt);

    // バックバッファの参照を解放
    swapChainResources_[0].Reset();
    swapChainResources_[1].Reset();
    depthStencilResource_.Reset();

    hr = swapChain_->ResizeBuffers(
        swapChainDesc_.BufferCount,
        UINT(WindowManager::winWidth_),
        UINT(WindowManager::winHeight_),
        swapChainDesc_.Format,
        0);
    assert(SUCCEEDED(hr));

    swapChainDesc_.Width = UINT(WindowManager::winWidth_);
    swapChainDesc_.Height = UINT(WindowManager::winHeight_);

    // 新しいバックバッファの取得とRTV再作成
    hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
    assert(SUCCEEDED(hr));
    hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
    assert(SUCCEEDED(hr));

    for (int i = 0; i < 2; ++i)
    {
        if (rtvAllocations_[i].index == UINT32_MAX)
        {
            uint32_t index = descriptorHeapManager_->GetRTVManager()->Allocate();
            rtvAllocations_[i] = { index, descriptorHeapManager_->GetRTVManager()->GetCPUHandleAt(index) };
        }
        device->CreateRenderTargetView(swapChainResources_[i].Get(), &rtvDesc_, rtvAllocations_[i].handle);
    }

    InitializeDepthStencilView(device);

    backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
}
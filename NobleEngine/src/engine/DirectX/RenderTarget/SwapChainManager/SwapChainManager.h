#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <DirectX/RenderTarget/RenderTargetStruct.h>

class DescriptorHeapManager;

class SwapChainManager
{
public:
	SwapChainManager(ID3D12Device* device, ID3D12CommandQueue* commandQueue, HWND hwnd, DescriptorHeapManager* descriptorHeapManager);
	~SwapChainManager();

	UINT GetCurrentBackBufferIndex() const { return backBufferIndex_; }
	RenderTarget* GetCurrentRenderTarget() const;

	const DXGI_SWAP_CHAIN_DESC1& GetSwapChainDesc() const { return swapChainDesc_; }
	const D3D12_RENDER_TARGET_VIEW_DESC& GetRtvDesc() const { return rtvDesc_; }

	void Present();
	void UpdateBackBufferIndex();

	void Resize(ID3D12Device* device, ID3D12CommandQueue* commandQueue);

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];
	std::unique_ptr<RenderTarget> renderTargets_[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;


	RTVManager::Allocation rtvAllocations_[2]{ {UINT32_MAX}, {UINT32_MAX} };
	DSVManager::Allocation dsvAllocation_{ UINT32_MAX };

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
	UINT backBufferIndex_;


	void InitializeSwapChainInternal(ID3D12Device* device, ID3D12CommandQueue* commandQueue, HWND hwnd);
	void InitializeRenderTargetView(ID3D12Device* device);
	void InitializeDepthStencilView(ID3D12Device* device);


	// 外部ポインタ
	ID3D12Device* device_;
	DescriptorHeapManager* descriptorHeapManager_ = nullptr;
};
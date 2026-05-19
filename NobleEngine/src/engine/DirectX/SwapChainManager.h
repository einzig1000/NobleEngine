#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "DirectX/DescriptorHeapManager/DescriptorHeapManager.h"

class SwapChainManager
{
public:
	SwapChainManager(ID3D12Device* device, ID3D12CommandQueue* commandQueue, HWND hwnd, DescriptorHeapManager* descriptorHeapManager);
	~SwapChainManager();

	UINT GetCurrentBackBufferIndex() const { return backBufferIndex_; }
	ID3D12Resource* GetCurrentBackBufferResource() const { return swapChainResources_[backBufferIndex_].Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle() const { return rtvAllocations_[backBufferIndex_].handle; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const { return mainDepthDSV_.handle; }

	const DXGI_SWAP_CHAIN_DESC1& GetSwapChainDesc() const { return swapChainDesc_; }
	const D3D12_RENDER_TARGET_VIEW_DESC& GetRtvDesc() const { return rtvDesc_; }

	void Present();
	void UpdateBackBufferIndex();

	void Resize(ID3D12Device* device, ID3D12CommandQueue* commandQueue);

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;


	RTVManager::Allocation rtvAllocations_[2]{ {UINT32_MAX}, {UINT32_MAX} };
	DSVManager::Allocation mainDepthDSV_{ UINT32_MAX };

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
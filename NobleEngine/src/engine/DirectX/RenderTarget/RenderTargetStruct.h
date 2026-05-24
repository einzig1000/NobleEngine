#pragma once
#include <DirectX/DescriptorHeapManager/DescriptorHeapManager.h>
#include <dxgiformat.h>
#include <d3d12.h>
#include <Windows.h>
#include <wrl/client.h>
#include <DirectX/DescriptorHeapManager/DSV/DSVManager.h>
#include <DirectX/DescriptorHeapManager/RTV/RTVManager.h>
#include <DirectX/DescriptorHeapManager/SRV_UAV/SRV_UAVManager.h>

struct RenderTarget
{
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	Microsoft::WRL::ComPtr<ID3D12Resource> dsvResource;
	RTVManager::Allocation rtvAlloc{};
	DSVManager::Allocation dsvAlloc{};
	SRV_UAVManager::Allocation srvAlloc{};
	UINT width = 0;
	UINT height = 0;
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
	D3D12_RESOURCE_STATES dsvState = D3D12_RESOURCE_STATE_COMMON;
};
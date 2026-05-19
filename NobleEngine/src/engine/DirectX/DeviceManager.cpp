#include "DeviceManager.h"
#include <vector>
#include <d3d12sdklayers.h>
#include <dxgi1_6.h> 
#include "Utilities/Logger/Logger.h"

#pragma comment(lib, "dxgi.lib") 
#pragma comment(lib, "d3d12.lib") 

DeviceManager::DeviceManager()
{
#ifdef _DEBUG
    EnableDebugLayer();
#endif
    InitializeDeviceInternal();

    Log("コンストラクタ実行成功 : DeviceManager");
}

DeviceManager::~DeviceManager()
{
    Log("デストラクタ実行成功 : DeviceManager");
}

void DeviceManager::EnableDebugLayer()
{
    Microsoft::WRL::ComPtr<ID3D12Debug1> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
}

void DeviceManager::InitializeDeviceInternal()
{
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    assert(SUCCEEDED(hr));

    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter;
    for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        DXGI_ADAPTER_DESC3 adapterDesc;
        hr = useAdapter->GetDesc3(&adapterDesc);
        assert(SUCCEEDED(hr));
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
        {
            break;
        }
    }

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
    };
    for (size_t i = 0; i < _countof(featureLevels); ++i)
    {
        hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device));
        if (SUCCEEDED(hr))
        {
            break;
        }
    }
    assert(device != nullptr);

#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
    if (device && SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
    {
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        D3D12_MESSAGE_ID denyIds[] = {
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        infoQueue->PushStorageFilter(&filter);
    }
#endif
}
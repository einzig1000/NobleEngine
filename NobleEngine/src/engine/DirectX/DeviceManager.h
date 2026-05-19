#pragma once
#include <d3d12sdklayers.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cassert>

class DeviceManager
{
public:
    DeviceManager();
    ~DeviceManager();

    ID3D12Device* GetDevice() const { return device.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    void EnableDebugLayer();
    void InitializeDeviceInternal();
};
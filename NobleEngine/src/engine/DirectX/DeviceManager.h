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

    ID3D12Device2* GetDevice() const { return device_.Get(); }

	bool IsMeshShaderSupported() const { return isMeshShaderSupported_; }

private:
	// デバイス
    Microsoft::WRL::ComPtr<ID3D12Device2> device_;

	// メッシュシェーダー対応のデバイスかどうか
	bool isMeshShaderSupported_ = false;

    void EnableDebugLayer();
    void InitializeDeviceInternal();
};
#pragma once
#include <d3d12.h>

/// <summary>
/// 
/// </summary>
class ViewportScissorManager
{
public:
    ViewportScissorManager();
    ~ViewportScissorManager();
    void Resize();

    const D3D12_VIEWPORT& GetViewport() { return viewport; }
    const D3D12_RECT& GetScissorRect() { return scissorRect; }

private:
    D3D12_VIEWPORT viewport;
    D3D12_RECT scissorRect;
};
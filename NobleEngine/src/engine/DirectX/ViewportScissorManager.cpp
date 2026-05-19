#include "DirectX/ViewportScissorManager.h"
#include "Utilities/Logger/Logger.h"
#include "Window/WindowManager.h"

ViewportScissorManager::ViewportScissorManager()
{
    viewport.Width = FLOAT(WindowManager::winWidth_);
    viewport.Height = FLOAT(WindowManager::winHeight_);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    scissorRect.left = 0;
    scissorRect.right = LONG(WindowManager::winWidth_);
    scissorRect.top = 0;
    scissorRect.bottom = LONG(WindowManager::winHeight_);

    Log("コンストラクタ実行成功 : ViewportScissorManager");
}

ViewportScissorManager::~ViewportScissorManager()
{
    Log("デストラクタ実行成功 : ViewportScissorManager");
}

void ViewportScissorManager::Resize()
{
    scissorRect.left = 0;
    scissorRect.right = LONG(WindowManager::winWidth_);
    scissorRect.top = 0;
    scissorRect.bottom = LONG(WindowManager::winHeight_);
    viewport.Width = FLOAT(WindowManager::winWidth_);
    viewport.Height = FLOAT(WindowManager::winHeight_);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
}

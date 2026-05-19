#include "DirectX/CommandContextManager.h"
#include "Utilities/Logger/Logger.h"

CommandContextManager::CommandContextManager(ID3D12Device* device)
{
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
    assert(SUCCEEDED(hr));

    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    assert(SUCCEEDED(hr));

    hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    assert(SUCCEEDED(hr));

    hr = commandList->Close();
    assert(SUCCEEDED(hr));

    Log("コンストラクタ実行成功 : CommandContextManager");
}

CommandContextManager::~CommandContextManager()
{
    Log("デストラクタ実行成功 : CommandContextManager");
}

void CommandContextManager::ResetCommandList()
{
    HRESULT hr = commandAllocator->Reset();
    assert(SUCCEEDED(hr));
    hr = commandList->Reset(commandAllocator.Get(), nullptr);
    assert(SUCCEEDED(hr));
}
#include "DirectX/CommandContextManager.h"
#include "Utilities/Logger/Logger.h"

CommandContextManager::CommandContextManager(ID3D12Device* device)
{
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
    assert(SUCCEEDED(hr));
    
    // フレーム数分の CommandAllocator を作成
    for (UINT i = 0; i < kFrameCount; ++i)
    {
        hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i]));
        assert(SUCCEEDED(hr));

		hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[i].Get(), nullptr, IID_PPV_ARGS(&commandList[i]));
		assert(SUCCEEDED(hr));

		hr = commandList[i]->Close();    // コマンドリストは最初クローズしておく
        assert(SUCCEEDED(hr));
    }

    Log("コンストラクタ実行成功 : CommandContextManager");
}

CommandContextManager::~CommandContextManager()
{
    Log("デストラクタ実行成功 : CommandContextManager");
}

void CommandContextManager::ResetCommandList(UINT frameIndex)
{
    HRESULT hr = commandAllocators[frameIndex]->Reset();
    assert(SUCCEEDED(hr));
    hr = commandList[frameIndex]->Reset(commandAllocators[frameIndex].Get(), nullptr);
    assert(SUCCEEDED(hr));
}

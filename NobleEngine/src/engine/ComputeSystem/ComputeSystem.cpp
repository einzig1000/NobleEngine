#include "ComputeSystem.h"
#include <DirectX/DirectXManager.h>
#include <RootBinding/RootBindingManager.h>

ComputeSystem::ComputeSystem(DirectXManager* dxManager, RootBindingManager* rootBindingManager)
	: dxManager_(dxManager), rootBindingManager_(rootBindingManager) {}

ComputeSystem::~ComputeSystem()
{}

void ComputeSystem::Reset()
{
	backBufferIndex_ = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();

	computeObjects_.clear();
}

void ComputeSystem::AddComputeObject(const ComputeObject* computeObject)
{
	computeObjects_.push_back(computeObject);
}

void ComputeSystem::Execute()
{
	auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);
	auto* sBufferManager = rootBindingManager_->GetStructuredBufferManager();

	// 1) Dispatch前：これから書き込むすべての出力バッファをUAV状態へ遷移
	for (const auto& computeObject : computeObjects_)
	{
		for (const auto& handle : computeObject->GetOutputHandles())
		{
			sBufferManager->TransitionToUAV(handle, cmdList);
		}
	}

	// 2) 全Dispatch
	for (const auto& computeObject : computeObjects_)
	{
		DispatchComputeObject(computeObject);
	}

	// 3) Dispatch後：全出力バッファをまとめてSRV読み取り状態へ遷移
	for (const auto& computeObject : computeObjects_)
	{
		for (const auto& handle : computeObject->GetOutputHandles())
		{
			sBufferManager->TransitionToSRV(handle, cmdList);
		}
	}
}

void ComputeSystem::DispatchComputeObject(const ComputeObject* computeObject)
{
	auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex_);
	auto* srvUavManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();

	const RootLayout* layout = computeObject->GetRootLayout();
	assert(layout && "SetupFromShaders()が呼ばれていません");

	// 1) RootSignatureセット
	cmdList->SetComputeRootSignature(layout->rootSignature);
	// 2) PSOセット
	cmdList->SetPipelineState(dxManager_->GetPipelineStateManager()->GetComputePipelineState(computeObject->psoConfig_, *layout));
	// 3) CBV・SRVセット
	const auto& values = computeObject->GetRootValues();
	for (size_t i = 0; i < layout->params.size(); ++i)
	{
		const auto& param = layout->params[i];
		const auto& value = values[i];

		if (param.paramType == ParamType::CBV)
		{
			assert(value.gpuAddress != 0);
			cmdList->SetComputeRootConstantBufferView(static_cast<UINT>(i), value.gpuAddress);
		}
		else if (param.paramType == ParamType::SRV)
		{
			assert(value.allocIndex != UINT32_MAX);
			cmdList->SetComputeRootDescriptorTable(static_cast<UINT>(i), srvUavManager->GetGPUHandleAt(value.allocIndex));
		}
		else if (param.paramType == ParamType::UAV)
		{
			assert(value.allocIndex != UINT32_MAX);
			cmdList->SetComputeRootDescriptorTable(static_cast<UINT>(i), srvUavManager->GetGPUHandleAt(value.allocIndex));
		}
	}

	cmdList->Dispatch(UINT(computeObject->size.x), UINT(computeObject->size.y), UINT(computeObject->size.z));
}


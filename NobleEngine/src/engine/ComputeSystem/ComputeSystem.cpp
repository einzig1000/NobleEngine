#include "ComputeSystem.h"
#include <DirectX/DirectXManager.h>
#include <RootBinding/StructuredBufferManager/StructuredBufferManager.h>

ComputeSystem::ComputeSystem(DirectXManager* dxManager, StructuredBufferManager* structuredBufferManager)
	: dxManager_(dxManager), structuredBufferManager_(structuredBufferManager)
{
	for (uint32_t i = 0; i < Constexprs::kFrameCount; ++i)
	{
		cbAllocators_[i].Initialize(dxManager_->GetDevice(), 8 * 1024 * 1024, L"FrameCBAllocator");
	}
}

ComputeSystem::~ComputeSystem()
{}

void ComputeSystem::Reset()
{
	// CBアロケータをリセット
	auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	cbAllocators_[backBufferIndex].Reset();

	computeObjects_.clear();
}

void ComputeSystem::AddComputeObject(const ComputeObject* computeObject)
{
	computeObjects_.push_back(computeObject);
}

void ComputeSystem::DispatchComputeObjects()
{
	auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);

	// 1) Dispatch前：これから書き込むすべての出力バッファをUAV状態へ遷移
	for (const auto& computeObject : computeObjects_)
	{
		for (const auto& handle : computeObject->GetOutputHandles())
		{
			structuredBufferManager_->TransitionToUAV(handle, cmdList);
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
			structuredBufferManager_->TransitionToSRV(handle, cmdList);
		}
	}
}

void ComputeSystem::DispatchComputeObject(const ComputeObject* computeObject)
{
	auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);
	auto& cb = cbAllocators_[backBufferIndex];
	auto* srvUavManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();

	// 1) RootSignatureセット
	cmdList->SetComputeRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature(computeObject->GetRootParams()).Get());
	// 2) PSOセット
	cmdList->SetPipelineState(dxManager_->GetPipelineStateManager()->GetComputePipelineState(computeObject->psoConfig_, computeObject->GetRootParams()).Get());
	// 3) CBV・SRVセット
	const auto& cpuStorage = computeObject->GetCpuStorage();
	const auto& rootParams = computeObject->GetRootParams();
	for (size_t i = 0; i < rootParams.size(); ++i)
	{
		const auto& param = rootParams[i];

		if (param.paramType == ParamType::CBV)
		{
			const auto alloc = cb.Allocate(param.sizeBytes);
			std::memcpy(alloc.cpu, cpuStorage.data() + param.offsetBytes, param.sizeBytes);
			cmdList->SetComputeRootConstantBufferView(static_cast<UINT>(i), alloc.gpu);
		}
		else if (param.paramType == ParamType::SRV)
		{
			assert(param.srvAllocIndex != UINT32_MAX);
			cmdList->SetComputeRootDescriptorTable(static_cast<UINT>(i), srvUavManager->GetGPUHandleAt(param.srvAllocIndex));
		}
		else if (param.paramType == ParamType::UAV)
		{
			assert(param.uavAllocIndex != UINT32_MAX);
			cmdList->SetComputeRootDescriptorTable(static_cast<UINT>(i), srvUavManager->GetGPUHandleAt(param.uavAllocIndex));
		}
	}

	cmdList->Dispatch(UINT(computeObject->size.x), UINT(computeObject->size.y), UINT(computeObject->size.z));
}

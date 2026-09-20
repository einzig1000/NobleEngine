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
	backBufferIndex_ = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	cbAllocators_[backBufferIndex_].Reset();

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

D3D12_GPU_VIRTUAL_ADDRESS ComputeSystem::GetCurrentFrameCbGpuAddress(size_t sizeBytes, const void* data)
{
	const auto alloc = cbAllocators_[backBufferIndex_].Allocate(sizeBytes);
	std::memcpy(alloc.cpu, data, static_cast<size_t>(sizeBytes));
	return alloc.gpu;
}

void ComputeSystem::DispatchComputeObject(const ComputeObject* computeObject)
{
	auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex_);
	auto* srvUavManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();

	// 1) RootSignatureセット
	cmdList->SetComputeRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature(computeObject->GetRootParams()).Get());
	// 2) PSOセット
	cmdList->SetPipelineState(dxManager_->GetPipelineStateManager()->GetComputePipelineState(computeObject->psoConfig_, computeObject->GetRootParams()).Get());
	// 3) CBV・SRVセット
	const auto& rootParams = computeObject->GetRootParams();
	for (size_t i = 0; i < rootParams.size(); ++i)
	{
		const auto& param = rootParams[i];

		if (param.paramType == ParamType::CBV)
		{
			assert(param.gpuAddress != 0);
			cmdList->SetComputeRootConstantBufferView(static_cast<UINT>(i), param.gpuAddress);
		}
		else if (param.paramType == ParamType::SRV)
		{
			assert(param.allocIndex != UINT32_MAX);
			cmdList->SetComputeRootDescriptorTable(static_cast<UINT>(i), srvUavManager->GetGPUHandleAt(param.allocIndex));
		}
		else if (param.paramType == ParamType::UAV)
		{
			assert(param.allocIndex != UINT32_MAX);
			cmdList->SetComputeRootDescriptorTable(static_cast<UINT>(i), srvUavManager->GetGPUHandleAt(param.allocIndex));
		}
	}

	cmdList->Dispatch(UINT(computeObject->size.x), UINT(computeObject->size.y), UINT(computeObject->size.z));
}

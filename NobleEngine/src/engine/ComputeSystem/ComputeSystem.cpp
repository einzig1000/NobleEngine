#include "ComputeSystem.h"
#include <DirectX/DirectXManager.h>

ComputeSystem::ComputeSystem(DirectXManager* dxManager)
	: dxManager_(dxManager)
{}

ComputeSystem::~ComputeSystem()
{}

void ComputeSystem::Reset()
{
	computeObjects_.clear();
}

void ComputeSystem::AddComputeObject(const ComputeObject* computeObject)
{
	computeObjects_.push_back(computeObject);
}

void ComputeSystem::DispatchComputeObjects()
{
	for (const auto& computeObject : computeObjects_)
	{
		DispatchComputeObject(computeObject);
	}
}

void ComputeSystem::DispatchComputeObject(const ComputeObject* computeObject)
{
	auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);
	auto& cb = cbAllocators_[backBufferIndex];
	auto* srvManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();

	// 1) RootSignatureセット
	cmdList->SetComputeRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature(computeObject->GetRootParams()).Get());
	// 2) PSOセット
	cmdList->SetPipelineState(dxManager_->GetPipelineStateManager()->GetComputePipelineState(computeObject->psoConfig_, computeObject->GetRootParams()).Get());
	// 3) CBV・SRVセット
	const auto& cpuStrage = computeObject->GetCpuStorage();
	const auto& rootParams = computeObject->GetRootParams();
	for (size_t i = 0; i < rootParams.size(); ++i)
	{
		const auto& param = rootParams[i];

		if (param.paramType == ParamType::CBV)
		{
			const auto alloc = cb.Allocate(param.sizeBytes);
			std::memcpy(alloc.cpu, cpuStrage.data() + param.offsetBytes, param.sizeBytes);
			cmdList->SetComputeRootConstantBufferView(static_cast<UINT>(i), alloc.gpu);
		}
		else if (param.paramType == ParamType::SRV)
		{
			assert(param.srvAllocIndex != UINT32_MAX);
			cmdList->SetComputeRootDescriptorTable(static_cast<UINT>(i), srvManager->GetGPUHandleAt(param.srvAllocIndex));
		}
	}

	cmdList->Dispatch(UINT(computeObject->size.x), UINT(computeObject->size.y), UINT(computeObject->size.z));
}

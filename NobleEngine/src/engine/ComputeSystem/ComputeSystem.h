#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <EngineDefinition/EngineConstexprs.h>
#include <ComputeSystem/ComputeObject/ComputeObject.h>
#include <RootBinding/FrameCbAllocator/FrameCbAllocator.h>

class DirectXManager;
class StructuredBufferManager;

class ComputeSystem
{
public:
	ComputeSystem(DirectXManager* dxManager, StructuredBufferManager* structuredBufferManager);
	~ComputeSystem();
	void Reset();
	// ComputeObjectを追加する
	void AddComputeObject(const ComputeObject* computeObject);
	// リストに追加されたComputeObjectをすべて実行する
	void DispatchComputeObjects();

	D3D12_GPU_VIRTUAL_ADDRESS GetCurrentFrameCbGpuAddress(size_t sizeBytes, const void* data);

private:
	DirectXManager* dxManager_ = nullptr;
	StructuredBufferManager* structuredBufferManager_ = nullptr;
	UINT backBufferIndex_ = 0;

	// 実際にコンピューターシェーダーをぶん回す
	void DispatchComputeObject(const ComputeObject* computeObject);

	FrameCbAllocator cbAllocators_[Constexprs::kFrameCount]{};
	std::vector<const ComputeObject*> computeObjects_{};

};


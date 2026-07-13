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

private:
	// 実際にコンピューターシェーダーをぶん回す
	void DispatchComputeObject(const ComputeObject* computeObject);

	DirectXManager* dxManager_ = nullptr;
	StructuredBufferManager* structuredBufferManager_ = nullptr;
	FrameCbAllocator cbAllocators_[Constexprs::kFrameCount]{};
	std::vector<const ComputeObject*> computeObjects_{};

};


#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <EngineDefinition/EngineConstexprs.h>
#include <ComputeSystem/ComputeObject/ComputeObject.h>
#include <DirectX/FrameCbAllocator/FrameCbAllocator.h>

class DirectXManager;

class ComputeSystem
{
private:
	FrameCbAllocator cbAllocators_[Constexprs::kFrameCount]{};

public:
	ComputeSystem(DirectXManager* dxManager);
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
	std::vector<const ComputeObject*> computeObjects_{};

};


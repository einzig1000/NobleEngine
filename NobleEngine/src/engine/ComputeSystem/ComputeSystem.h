#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <EngineDefinition/EngineConstexprs.h>
#include <ComputeSystem/ComputeObject/ComputeObject.h>

class DirectXManager;
class RootBindingManager;

class ComputeSystem
{
public:
	ComputeSystem(DirectXManager* dxManager, RootBindingManager* rootBindingManager);
	~ComputeSystem();
	void Reset();
	// ComputeObjectを追加する
	void AddComputeObject(const ComputeObject* computeObject);
	// リストに追加されたComputeObjectをすべて実行する
	void Execute();

private:
	DirectXManager* dxManager_ = nullptr;
	RootBindingManager* rootBindingManager_ = nullptr;
	UINT backBufferIndex_ = 0;

	// 実際にコンピューターシェーダーをぶん回す
	void DispatchComputeObject(const ComputeObject* computeObject);

	std::vector<const ComputeObject*> computeObjects_{};

};


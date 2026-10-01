#pragma once
#include "ConstantBuffer/ConstantBufferManager.h"
#include "StructuredBuffer/StructuredBufferManager.h"

class DirectXManager;

class RootBindingManager
{
public:
	RootBindingManager(DirectXManager* dxManager);
	~RootBindingManager();
	void Reset();
	ConstantBufferManager* GetConstantBufferManager() { return cbManager_.get(); }
	StructuredBufferManager* GetStructuredBufferManager() { return sbManager_.get(); }


private:
	std::unique_ptr<ConstantBufferManager> cbManager_;
	std::unique_ptr<StructuredBufferManager> sbManager_;
};


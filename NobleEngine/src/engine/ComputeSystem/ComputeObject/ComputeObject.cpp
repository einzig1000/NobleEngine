#include "ComputeObject.h"
#include <Engine.h>
#include <DirectX/DirectXManager.h>
#include <DirectX/Pipeline/ShaderReflectionHelper/ShaderReflectionHelper.h>
#include <Utilities/Converter/StringConverter/StringConverter.h>
#include <Utilities/Logger/Logger.h>
#include <ComputeSystem/ComputeSystem.h>
#include <RootBinding/RootBindingManager.h>
#include <cstdint>

void ComputeObject::SetupFromShaders()
{
	rootParams_.clear();
	rootParamHashToIndexMap_.clear();
	outputHandles_.clear();

	std::wstring csPath = StringConverter::Convert(psoConfig_.cs);
	const auto& csParams = Engine::Instance().GetDirectXManager()->GetPipelineStateManager()->GetShaderRootParams(csPath.c_str(), L"cs_6_6", ShaderType::ComputeShader);

	// CS の CBV / SRV を反映
	rootParams_ = csParams;

#ifdef _DEBUG

	// デバッグビルドではハッシュの衝突がないか確認する
	for (size_t i = 0; i < rootParams_.size(); ++i)
	{
		const auto& param = rootParams_[i];
		if (rootParamHashToIndexMap_.find(param.hash) != rootParamHashToIndexMap_.end())
		{
			Log("RenderObject::SetupFromShaders() ルートパラメータのハッシュが衝突しました");
			__debugbreak();
		}
		else
		{
			rootParamHashToIndexMap_[param.hash] = i;
		}
	}

#else

	for (size_t i = 0; i < rootParams_.size(); ++i)
	{
		rootParamHashToIndexMap_[rootParams_[i].hash] = i;
	}

#endif
}

void ComputeObject::SetBRegisterData(const uint32_t key, const void* data, uint32_t space)
{
	RootParam tempParam{};
	tempParam.paramType = ParamType::CBV;
	tempParam.shaderType = ShaderType::ComputeShader;
	tempParam.key = key;
	tempParam.registerSpace = space;
	tempParam.ComputeHash();

	const auto& it = rootParamHashToIndexMap_.find(tempParam.hash);
	if (it == rootParamHashToIndexMap_.end()) return;
	auto& param = rootParams_.at(it->second);

	param.gpuAddress = Engine::Instance().GetRootBindingManager()->GetConstantBufferManager()->GetCurrentFrameCbGpuAddress(param.sizeBytes, data);
	return;
}

void ComputeObject::SetTRegisterData(const uint32_t key, const uint32_t allocIndex, uint32_t space)
{
	RootParam tempParam{};
	tempParam.paramType = ParamType::SRV;
	tempParam.shaderType = ShaderType::ComputeShader;
	tempParam.key = key;
	tempParam.registerSpace = space;
	tempParam.ComputeHash();

	const auto& it = rootParamHashToIndexMap_.find(tempParam.hash);
	if (it == rootParamHashToIndexMap_.end()) return;
	auto& param = rootParams_.at(it->second);

	param.allocIndex = allocIndex;
}

void ComputeObject::SetURegisterData(const uint32_t key, const uint32_t allocIndex, uint32_t space)
{
	RootParam tempParam{};
	tempParam.paramType = ParamType::UAV;
	tempParam.shaderType = ShaderType::ComputeShader;
	tempParam.key = key;
	tempParam.registerSpace = space;
	tempParam.ComputeHash();

	const auto& it = rootParamHashToIndexMap_.find(tempParam.hash);
	if (it == rootParamHashToIndexMap_.end()) return;
	auto& param = rootParams_.at(it->second);

	param.allocIndex = allocIndex;
}

void ComputeObject::Dispatch()
{
	Engine::Instance().GetComputeSystem()->AddComputeObject(this);
}

#include "ComputeObject.h"
#include <Engine.h>
#include <DirectX/DirectXManager.h>
#include <DirectX/Pipeline/ShaderReflectionHelper/ShaderReflectionHelper.h>
#include <Utilities/Converter/StringConverter/StringConverter.h>
#include <cstring>
#include <cstdint>

void ComputeObject::SetupFromShaders()
{
	rootParams_.clear();
	rootParamHashToIndexMap_.clear();
	cpuStorage_.clear();
	outputHandles_.clear();

	uint32_t cbvSizeOffset = 0;

	std::wstring csPath = StringConverter::Convert(psoConfig_.cs);
	auto csBlob = Engine::Instance().GetDirectXManager()->GetPipelineStateManager()->GetShaderBlob(csPath.c_str(), L"cs_6_6");

	// CS の CBV / SRV を反映
	ShaderReflection::BuildRootParamsFromShader(csBlob.Get(), ShaderType::ComputeShader, rootParams_, cbvSizeOffset);

	for (size_t i = 0; i < rootParams_.size(); ++i)
	{
		rootParamHashToIndexMap_[rootParams_[i].hash] = i;
	}

	cpuStorage_.resize(cbvSizeOffset);
}

void ComputeObject::SetCBufferData(const uint32_t key, ShaderType shaderType, const void* data, uint32_t space)
{
	RootParam tempParam{};
	tempParam.paramType = ParamType::CBV;
	tempParam.shaderType = shaderType;
	tempParam.key = key;
	tempParam.registerSpace = space;
	tempParam.ComputeHash();

	const auto& it = rootParamHashToIndexMap_.find(tempParam.hash);
	if (it == rootParamHashToIndexMap_.end()) return;
	const auto& param = rootParams_.at(it->second);

	if (param.paramType == ParamType::CBV && param.shaderType == shaderType && param.key == key)
	{
		std::memcpy(cpuStorage_.data() + param.offsetBytes, data, param.sizeBytes);
		return;
	}
}

void ComputeObject::SetSBufferData(const uint32_t key, ShaderType shaderType, const uint32_t srvAllocIndex, uint32_t space)
{
	RootParam tempParam{};
	tempParam.paramType = ParamType::SRV;
	tempParam.shaderType = shaderType;
	tempParam.key = key;
	tempParam.registerSpace = space;
	tempParam.ComputeHash();

	const auto& it = rootParamHashToIndexMap_.find(tempParam.hash);
	if (it == rootParamHashToIndexMap_.end()) return;
	auto& param = rootParams_.at(it->second);

	if (param.paramType == ParamType::SRV && param.shaderType == shaderType && param.key == key)
	{
		param.srvAllocIndex = srvAllocIndex;
	}
}

void ComputeObject::SetUAVData(const uint32_t key, ShaderType shaderType, const uint32_t uavAllocIndex, uint32_t space)
{
	RootParam tempParam{};
	tempParam.paramType = ParamType::UAV;
	tempParam.shaderType = shaderType;
	tempParam.key = key;
	tempParam.registerSpace = space;
	tempParam.ComputeHash();

	const auto& it = rootParamHashToIndexMap_.find(tempParam.hash);
	if (it == rootParamHashToIndexMap_.end()) return;
	auto& param = rootParams_.at(it->second);

	if (param.paramType == ParamType::UAV && param.shaderType == shaderType && param.key == key)
	{
		param.uavAllocIndex = uavAllocIndex;
	}
}

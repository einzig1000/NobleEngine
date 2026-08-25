#include "RenderObject.h"
#include <Engine.h>
#include <DirectX/DirectXManager.h>
#include <DirectX/Pipeline/ShaderReflectionHelper/ShaderReflectionHelper.h>
#include <Utilities/Converter/StringConverter/StringConverter.h>
#include <Utilities/Logger/Logger.h>
#include <DrawSystem/DrawSystem.h>
#include <cstring>
#include <cstdint>

void RenderObject::SetupFromShaders()
{
	rootParams_.clear();
	rootParamHashToIndexMap_.clear();
	cpuStorage_.clear();

	uint32_t cbvSizeOffset = 0;

	if (psoConfig_.as != "unknown")
	{
		std::wstring asPath = StringConverter::Convert(psoConfig_.as);
		auto asBlob = Engine::Instance().GetDirectXManager()->GetPipelineStateManager()->GetShaderBlob(asPath.c_str(), L"as_6_6");

		// ASのルートパラメータを取得
		ShaderReflection::BuildRootParamsFromShader(asBlob.Get(), ShaderType::AmplificationShader, rootParams_, cbvSizeOffset);
	}
	if (psoConfig_.vs != "unknown")
	{
		std::wstring vsPath = StringConverter::Convert(psoConfig_.vs);
		auto vsBlob = Engine::Instance().GetDirectXManager()->GetPipelineStateManager()->GetShaderBlob(vsPath.c_str(), L"vs_6_6");

		// VSのルートパラメータを取得
		ShaderReflection::BuildRootParamsFromShader(vsBlob.Get(), ShaderType::VertexShader, rootParams_, cbvSizeOffset);
	}
	else if (psoConfig_.ms != "unknown")
	{
		std::wstring msPath = StringConverter::Convert(psoConfig_.ms);
		auto msBlob = Engine::Instance().GetDirectXManager()->GetPipelineStateManager()->GetShaderBlob(msPath.c_str(), L"ms_6_6");

		// MSのルートパラメータを取得
		ShaderReflection::BuildRootParamsFromShader(msBlob.Get(), ShaderType::MeshShader, rootParams_, cbvSizeOffset);
	}
	{
		std::wstring psPath = StringConverter::Convert(psoConfig_.ps);
		auto psBlob = Engine::Instance().GetDirectXManager()->GetPipelineStateManager()->GetShaderBlob(psPath.c_str(), L"ps_6_6");

		// PSのルートパラメータを取得
		ShaderReflection::BuildRootParamsFromShader(psBlob.Get(), ShaderType::PixelShader, rootParams_, cbvSizeOffset);
	}

	
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

	cpuStorage_.resize(cbvSizeOffset);
}

void RenderObject::SetCBufferData(const uint32_t key, ShaderType shaderType, const void* data, uint32_t space)
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

	std::memcpy(cpuStorage_.data() + param.offsetBytes, data, param.sizeBytes);
	return;
}

void RenderObject::SetSBufferData(const uint32_t key, ShaderType shaderType, const uint32_t srvAllocIndex, uint32_t space)
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

	param.srvAllocIndex = srvAllocIndex;
}


void RenderObject::Draw(int32_t renderTargetID, std::vector<int32_t> deps) const
{
	Engine::Instance().GetDrawSystem()->AddDrawList(this, renderTargetID, deps);
}

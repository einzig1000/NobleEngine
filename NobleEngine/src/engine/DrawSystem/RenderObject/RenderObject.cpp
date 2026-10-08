#include "RenderObject.h"
#include <Engine.h>
#include <DirectX/DirectXManager.h>
#include <DirectX/Pipeline/ShaderReflectionHelper/ShaderReflectionHelper.h>
#include <Utilities/Converter/StringConverter/StringConverter.h>
#include <Utilities/Logger/Logger.h>
#include <DrawSystem/DrawSystem.h>
#include <RootBinding/RootBindingManager.h>
#include <cstring>
#include <cstdint>

void RenderObject::SetupFromShaders()
{
	rootParams_.clear();
	rootParamHashToIndexMap_.clear();

	auto* psoManager = Engine::Instance().GetDirectXManager()->GetPipelineStateManager();

	if (psoConfig_.as != "unknown")
	{
		std::wstring asPath = StringConverter::Convert(psoConfig_.as);
		const auto& asParams = psoManager->GetShaderRootParams(asPath.c_str(), L"as_6_6", ShaderType::AmplificationShader);

		// ASのルートパラメータを取得
		rootParams_.insert(rootParams_.end(), asParams.begin(), asParams.end());
	}
	if (psoConfig_.vs != "unknown")
	{
		std::wstring vsPath = StringConverter::Convert(psoConfig_.vs);
		const auto& vsParams = psoManager->GetShaderRootParams(vsPath.c_str(), L"vs_6_6", ShaderType::VertexShader);

		// VSのルートパラメータを取得
		rootParams_.insert(rootParams_.end(), vsParams.begin(), vsParams.end());
	}
	else if (psoConfig_.ms != "unknown")
	{
		std::wstring msPath = StringConverter::Convert(psoConfig_.ms);
		const auto& msParams = psoManager->GetShaderRootParams(msPath.c_str(), L"ms_6_6", ShaderType::MeshShader);

		// MSのルートパラメータを取得
		rootParams_.insert(rootParams_.end(), msParams.begin(), msParams.end());
	}
	{
		std::wstring psPath = StringConverter::Convert(psoConfig_.ps);
		const auto& psParams = psoManager->GetShaderRootParams(psPath.c_str(), L"ps_6_6", ShaderType::PixelShader);

		// PSのルートパラメータを取得
		rootParams_.insert(rootParams_.end(), psParams.begin(), psParams.end());
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
}

void RenderObject::SetBRegisterData(const uint32_t key, ShaderType shaderType, const void* data, uint32_t space)
{
	RootParam tempParam{};
	tempParam.paramType = ParamType::CBV;
	tempParam.shaderType = shaderType;
	tempParam.key = key;
	tempParam.registerSpace = space;
	tempParam.ComputeHash();

	const auto& it = rootParamHashToIndexMap_.find(tempParam.hash);
	if (it == rootParamHashToIndexMap_.end()) return;
	auto& param = rootParams_.at(it->second);

	param.gpuAddress = Engine::Instance().GetRootBindingManager()->GetConstantBufferManager()->GetCurrentFrameCbGpuAddress(param.sizeBytes, data);
	return;
}

void RenderObject::SetTRegisterData(const uint32_t key, ShaderType shaderType, const uint32_t allocIndex, uint32_t space)
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

	param.allocIndex = allocIndex;
}


void RenderObject::Draw(int32_t renderTargetID, std::vector<int32_t> deps) const
{
	Engine::Instance().GetDrawSystem()->AddDrawList(this, renderTargetID, deps);
}

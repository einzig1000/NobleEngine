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
	outputHandles_.clear();

	layout_ = Engine::Instance().GetDirectXManager()->GetPipelineStateManager()->GetComputeRootLayout(psoConfig_);

	// 値はレイアウトと同じ並びで用意する
	rootValues_.assign(layout_->params.size(), RootParamValue{});
	for (size_t i = 0; i < layout_->params.size(); ++i)
	{
		// Bindless配列はヒープの先頭を指しておけばよいので、最初から0を入れておく
		if (layout_->params[i].isUnbounded)
		{
			rootValues_[i].allocIndex = 0;
		}
	}
}

void ComputeObject::SetBRegisterData(const uint32_t key, const void* data, uint32_t space)
{
	if (!layout_) return;
	const int32_t index = layout_->Find(ParamType::CBV, ShaderType::ComputeShader, key, space);
	if (index < 0) return;

	rootValues_[index].gpuAddress = Engine::Instance().GetRootBindingManager()->GetConstantBufferManager()->GetCurrentFrameCbGpuAddress(layout_->params[index].sizeBytes, data);
}

void ComputeObject::SetTRegisterData(const uint32_t key, const uint32_t allocIndex, uint32_t space)
{
	if (!layout_) return;
	const int32_t index = layout_->Find(ParamType::SRV, ShaderType::ComputeShader, key, space);
	if (index < 0) return;

	rootValues_[index].allocIndex = allocIndex;
}

void ComputeObject::SetURegisterData(const uint32_t key, const uint32_t allocIndex, uint32_t space)
{
	if (!layout_) return;
	const int32_t index = layout_->Find(ParamType::UAV, ShaderType::ComputeShader, key, space);
	if (index < 0) return;

	rootValues_[index].allocIndex = allocIndex;
}

void ComputeObject::Dispatch()
{
	Engine::Instance().GetComputeSystem()->AddComputeObject(this);
}

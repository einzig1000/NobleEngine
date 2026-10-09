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
	layout_ = Engine::Instance().GetDirectXManager()->GetPipelineStateManager()->GetGraphicsRootLayout(psoConfig_);

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

void RenderObject::SetBRegisterData(const uint32_t key, ShaderType shaderType, const void* data, uint32_t space)
{
	if (!layout_) return;
	const int32_t index = layout_->Find(ParamType::CBV, shaderType, key, space);
	if (index < 0) return;

	rootValues_[index].gpuAddress = Engine::Instance().GetRootBindingManager()->GetConstantBufferManager()->GetCurrentFrameCbGpuAddress(layout_->params[index].sizeBytes, data);
}

void RenderObject::SetTRegisterData(const uint32_t key, ShaderType shaderType, const uint32_t allocIndex, uint32_t space)
{
	if (!layout_) return;
	const int32_t index = layout_->Find(ParamType::SRV, shaderType, key, space);
	if (index < 0) return;

	rootValues_[index].allocIndex = allocIndex;
}


void RenderObject::Draw(int32_t renderTargetID, std::vector<int32_t> deps) const
{
	Engine::Instance().GetDrawSystem()->AddDrawList(this, renderTargetID, deps);
}

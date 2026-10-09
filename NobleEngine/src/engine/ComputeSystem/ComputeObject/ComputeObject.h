#pragma once
#include <vector>
#include <unordered_map>
#include <DirectX/PipeLine/RenderPipelineTypes.h>
#include <EngineDefinition/EngineDefinition.h>

class ComputeObject
{
public:
	void Dispatch();

	void SetupFromShaders();

	void SetBRegisterData(const uint32_t key, const void* data, uint32_t space = 0);
	void SetTRegisterData(const uint32_t key, const uint32_t allocIndex, uint32_t space = 0);
	void SetURegisterData(const uint32_t key, const uint32_t allocIndex, uint32_t space = 0);

	// このComputeObjectが書き込む出力バッファをバリア管理のために登録しておく
	void RegisterOutput(int32_t handle) { outputHandles_.push_back(handle); }
	const std::vector<int32_t>& GetOutputHandles() const { return outputHandles_; }

	const RootLayout* GetRootLayout() const { return layout_; }
	const std::vector<RootParamValue>& GetRootValues() const { return rootValues_; }

	ComputePSOConfig psoConfig_;

	Vector3int size = { 1, 1, 1 };

private:
	// ルートパラメータの形(同じシェーダーを使うオブジェクト同士で共有。所有はPipelineStateManager)
	const RootLayout* layout_ = nullptr;
	// ルートパラメータの値(layout_->params と同じ並び)
	std::vector<RootParamValue> rootValues_{};

	std::vector<int32_t> outputHandles_{};
};
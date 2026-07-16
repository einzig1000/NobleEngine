#pragma once
#include <vector>
#include <unordered_map>
#include <DirectX/PipeLine/RenderPipelineTypes.h>
#include <EngineDefinition/EngineDefinition.h>

class ComputeObject
{
public:
	void SetupFromShaders();

	void SetCBufferData(const uint32_t key, const void* data, uint32_t space = 0);
	void SetSBufferData(const uint32_t key, const uint32_t srvAllocIndex, uint32_t space = 0);
	void SetUAVData(const uint32_t key, const uint32_t uavAllocIndex, uint32_t space = 0);

	void Dispatch();

	// このComputeObjectが書き込む出力バッファをバリア管理のために登録しておく
	void RegisterOutput(int32_t handle) { outputHandles_.push_back(handle); }
	const std::vector<int32_t>& GetOutputHandles() const { return outputHandles_; }

	const std::vector<RootParam>& GetRootParams() const { return rootParams_; }
	const std::vector<uint8_t>& GetCpuStorage() const { return cpuStorage_; }

	ComputePSOConfig psoConfig_;

	Vector3int size = { 1, 1, 1 };

private:
	// RootParameterにいれるものリスト。データ本体は入ってない。
	std::vector<RootParam> rootParams_{};
	std::unordered_map<uint32_t, size_t> rootParamHashToIndexMap_{};

	// CBV用のストレージ。uint8_tのただのバイト列で保持。読みとる時はreinterpret_castで型を戻すイメージ。すべての情報を型に依存せずまとめて管理するためのもの。
	std::vector<uint8_t> cpuStorage_{};

	std::vector<int32_t> outputHandles_{};
};


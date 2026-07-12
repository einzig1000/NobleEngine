#pragma once
#include <string>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <unordered_map>
#include <DirectX/PipeLine/RenderPipelineTypes.h>
#include <DirectX/DescriptorHeapManager/SRV_UAV/SRV_UAVManager.h>
#include <EngineDefinition/EngineDefinition.h>
#include <EngineDefinition/EngineConstexprs.h>


class ComputeObject
{
public:
	struct DynamicSRVData
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> buffers[Constexprs::kFrameCount];
		void* mappedData[Constexprs::kFrameCount] = { nullptr };
		SRV_UAVManager::Allocation srvAllocations[Constexprs::kFrameCount];
	};

	struct OutputBufferData
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
		SRV_UAVManager::Allocation uavAllocation;
		SRV_UAVManager::Allocation srvAllocation; // 描画側が読むためのSRV
	};

	void SetupFromShaders();

	void SetCBufferData(const uint32_t key, ShaderType shaderType, const void* data, uint32_t space = 0);
	void SetSBufferData(const uint32_t key, ShaderType shaderType, const void* data, size_t elementSize, size_t elementCount, uint32_t space = 0);

	const std::vector<RootParam>& GetRootParams() const { return rootParams_; }
	const std::vector<uint8_t>& GetCpuStorage() const { return cpuStorage_; }

	ComputePSOConfig psoConfig_;

	Vector3int size = { 1, 1, 1 };

private:
	// RootParameterにいれるものリスト。CBVもSRVもここで管理する
	std::vector<RootParam> rootParams_{};
	std::unordered_map<uint32_t, size_t> rootParamHashToIndexMap_{};

	// CBV用のストレージ。uint8_tのただのバイト列で保持。読みとる時はreinterpret_castで型を戻すイメージ。すべての情報を型に依存せずまとめて管理するためのもの。
	std::vector<uint8_t> cpuStorage_{};

	// 動的SRV用のストレージ。CreateSRV() で確保し、SetBufferData() で直接GPUへ書き込む
	std::vector<DynamicSRVData> dynamicSrvStorage_{};

	// UAV用のストレージ。
	std::vector<OutputBufferData> outputBufferStorage_{};
};


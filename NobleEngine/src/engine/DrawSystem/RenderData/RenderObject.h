#pragma once
#include <string>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <unordered_map>
#include <DirectX/RenderTextureManager/RenderTextureID.h>
#include <DirectX/PipeLine/RenderPipelineTypes.h>
#include <DirectX/DescriptorHeapManager/SRV_UAV/SRV_UAVManager.h>

/// <summary>
// ・PSO 設定（どのシェーダ・どのブレンド・どのラスタライザか）
// ・ルートパラメータ（CBV / SRV）とその中身
// だけを持つ描画オブジェクト。描画に必要な情報はここに集約するイメージ。
/// </summary>
class RenderObject
{
public:
	static constexpr uint32_t kMaxFramesInFlight = 2;

	struct DynamicSRVData
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> buffers[kMaxFramesInFlight];
		void* mappedData[kMaxFramesInFlight] = { nullptr };
		SRV_UAVManager::Allocation srvAllocations[kMaxFramesInFlight];
	};

	void Draw(RenderTextureID renderTextureID = RenderTextureID::Test) const;
	void TestPostEffectDraw(RenderTextureID renderTextureID = RenderTextureID::PreBackBuffer) const;

	void SetupFromShaders();

	void SetCBufferData(const uint32_t key, ShaderType shaderType, const void* data, uint32_t space = 0);
	void SetSBufferData(const uint32_t key, ShaderType shaderType, const void* data, size_t elementSize, size_t elementCount, uint32_t space = 0);

	const std::vector<RootParam>& GetRootParams() const { return rootParams_; }
	const std::vector<uint8_t>& GetCpuStorage() const { return cpuStorage_; }

public:
	// PSO設定
	PSOConfig psoConfig_{};
	// インスタンス数
	uint32_t instanceNum_ = 1;

	int32_t modelID_ = -1;
	int32_t textureID_ = -1;

private:
	// RootParameterにいれるものリスト。CBVもSRVもここで管理する
	std::vector<RootParam> rootParams_{};
	std::unordered_map<uint32_t, size_t> rootParamHashToIndexMap_{};

	// CBV用のストレージ。uint8_tのただのバイト列で保持。読みとる時はreinterpret_castで型を戻すイメージ。すべての情報を型に依存せずまとめて管理するためのもの。
	std::vector<uint8_t> cpuStorage_{};

	// 動的SRV用のストレージ。CreateSRV() で確保し、SetBufferData() で直接GPUへ書き込む
	std::vector<DynamicSRVData> dynamicSrvStorage_{};

	// デバッグ用
	std::vector<std::string> debugNames_{};
};
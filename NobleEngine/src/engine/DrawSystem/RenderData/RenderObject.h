#pragma once
#include <vector>
#include <unordered_map>
#include <DirectX/PipeLine/RenderPipelineTypes.h>
#include <EngineDefinition/EngineConstexprs.h>

/// <summary>
// ・PSO 設定（どのシェーダー・どのブレンド・どのラスタライザか）
// ・ルートパラメータ（CBV / SRV）とその中身
// だけを持つ描画オブジェクト。描画に必要な情報はここに集約するイメージ。
/// </summary>
class RenderObject
{
public:
	/// <summary>
	/// 描画コール
	/// </summary>
	/// <param name="renderTargetID">このオブジェクトを描画するRTのID</param>
	/// <param name="deps">描画前に完了している必要があるRTのID一覧</param>
	void Draw(int32_t renderTargetID = -1, std::vector<int32_t> deps = {}) const;

	void SetupFromShaders();

	void SetCBufferData(const uint32_t key, ShaderType shaderType, const void* data, uint32_t space = 0);
	void SetSBufferData(const uint32_t key, ShaderType shaderType, const uint32_t srvAllocIndex, uint32_t space = 0);

	const std::vector<RootParam>& GetRootParams() const { return rootParams_; }
	const std::vector<uint8_t>& GetCpuStorage() const { return cpuStorage_; }

public:
	// PSO設定
	GraphicsPSOConfig psoConfig_{};
	// インスタンス数
	uint32_t instanceNum_ = 1;
	// モデルID
	int32_t modelID_ = -1;

	std::string name_ = "RenderObject";

private:
	// RootParameterにいれるものリスト。CBVもSRVもここで管理する
	std::vector<RootParam> rootParams_{};

	// ハッシュをキーにして紐づける
	std::unordered_map<uint32_t, size_t> rootParamHashToIndexMap_{};

	// CBV用のストレージ。uint8_tのただのバイト列で保持。読みとる時はreinterpret_castで型を戻すイメージ。すべての情報を型に依存せずまとめて管理するためのもの。
	std::vector<uint8_t> cpuStorage_{};
};
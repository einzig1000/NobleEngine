#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxcapi.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "DirectX/PipeLine/RenderPipelineTypes.h"


class PipelineStateManager
{
public:
    PipelineStateManager(ID3D12Device* device);
    ~PipelineStateManager();

    // ルートシグネチャキャッシュにあればそれを返す。なければ生成してキャッシュに保存してから返す
    Microsoft::WRL::ComPtr<ID3D12RootSignature> GetOrCreateRootSignature(const std::vector<RootParam>& params);
	// PSOキャッシュにあればそれを返す。なければ生成してキャッシュに保存してから返す
    Microsoft::WRL::ComPtr<ID3D12PipelineState> GetOrCreateGraphicsPipelineState(const PSOConfig& psoConfig, const std::vector<RootParam>& params);
	// シェーダーキャッシュにあればそれを返す。なければコンパイルしてキャッシュに保存してから返す
    Microsoft::WRL::ComPtr<IDxcBlob> GetOrCompileShader(const wchar_t* path, const wchar_t* target);

private:
	// ルートシグネチャ生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSignature(const std::vector<RootParam>& params);
	// パイプラインステート生成
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreatePipelineState(const PSOConfig& cfg, const std::vector<RootParam>& params);
	// シェーダーコンパイル
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

    // シェーダーキャッシュ
    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<IDxcBlob>> shaderCache_;
    // ルートシグネチャキャッシュ
    std::unordered_map<size_t, Microsoft::WRL::ComPtr<ID3D12RootSignature>> rootSignatureCache_;
    // PSOキャッシュ
    std::unordered_map<size_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> psoCache_;

    // シェーダー系
    void InitializeDxc();
    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

    // 外部ポインタ
    ID3D12Device* device_ = nullptr;


	D3D12_SHADER_VISIBILITY GetShaderVisibilityFromShaderType(ShaderType shaderType);

};
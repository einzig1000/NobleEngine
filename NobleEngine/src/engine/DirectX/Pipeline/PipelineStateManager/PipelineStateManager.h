#pragma once
#include <d3d12.h>
#include <externals/DirectXTex/d3dx12.h>
#include <wrl.h>
#include <dxcapi.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <DirectX/Pipeline/RenderPipelineTypes.h>

struct CD3DX12PipelineStateStream
{
    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
    CD3DX12_PIPELINE_STATE_STREAM_VS VS;                                   // VS用
    CD3DX12_PIPELINE_STATE_STREAM_MS pMS;                                  // MS用
	CD3DX12_PIPELINE_STATE_STREAM_AS pAS;                                  // AS用
    CD3DX12_PIPELINE_STATE_STREAM_PS pPS;                                  // 共通
    CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;                // VS用
    CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;// VS用
    CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC pBlend;                       // 共通
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL pDepthStencil;             // 共通
    CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS pRTVFormats;       // 共通
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT pDSVFormat;         // 共通
    CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER pRasterizer;                  // 共通
};

class PipelineStateManager
{
public:
    PipelineStateManager(ID3D12Device2* device);
    ~PipelineStateManager();

	// ルートシグネチャの取得
    Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature(const std::vector<RootParam>& params);
    // GraphicsPSO取得
    Microsoft::WRL::ComPtr<ID3D12PipelineState> GetGraphicsPipelineState(const GraphicsPSOConfig& psoConfig, const std::vector<RootParam>& params);
    // ComputePSO取得
    Microsoft::WRL::ComPtr<ID3D12PipelineState> GetComputePipelineState(const ComputePSOConfig& psoConfig, const std::vector<RootParam>& params);
	// シェーダーBlobの取得
    Microsoft::WRL::ComPtr<IDxcBlob> GetShaderBlob(const wchar_t* path, const wchar_t* target);

private:
	// ルートシグネチャ生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSignature(const std::vector<RootParam>& params);
	// GraphicsPSO生成
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipelineState(const GraphicsPSOConfig& cfg, const std::vector<RootParam>& params);
	// ComputePSO生成
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateComputePipelineState(const ComputePSOConfig& cfg, const std::vector<RootParam>& params);
	// シェーダーコンパイル
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

    // シェーダーキャッシュ
    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<IDxcBlob>> shaderCache_;
    // ルートシグネチャキャッシュ
    std::unordered_map<size_t, Microsoft::WRL::ComPtr<ID3D12RootSignature>> rootSignatureCache_;
    // GraphicsPSOキャッシュ
    std::unordered_map<size_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> graphicsPsoCache_;
    // ComputePSOキャッシュ
    std::unordered_map<size_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> computePsoCache_;

    // シェーダー系
    void InitializeDxc();
    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

    // 外部ポインタ
    ID3D12Device2* device_ = nullptr;


	D3D12_SHADER_VISIBILITY GetShaderVisibilityFromShaderType(ShaderType shaderType);

};
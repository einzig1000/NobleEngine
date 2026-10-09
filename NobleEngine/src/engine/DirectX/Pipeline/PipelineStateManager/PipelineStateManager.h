#pragma once
#include <d3d12.h>
#include <externals/DirectXTex/d3dx12.h>
#include <wrl.h>
#include <dxcapi.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <DirectX/Pipeline/RenderPipelineTypes.h>
#include <optional>

class PipelineStateManager
{
private:

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

    // シェーダー1つ分のキャッシュ
    struct ShaderCacheEntry
    {
        Microsoft::WRL::ComPtr<IDxcBlob> blob;
        // リフレクション結果。バインドが0個のシェーダーもあるので、空vectorと「未作成」をoptionalで区別する
        std::optional<std::vector<RootParam>> rootParams;
    };

public:
    PipelineStateManager(ID3D12Device2* device);
    ~PipelineStateManager();

    // ルートレイアウトの取得(シェーダーの組み合わせごとに1つ。ルートシグネチャもこの時に作る)
    const RootLayout* GetGraphicsRootLayout(const GraphicsPSOConfig& psoConfig);
    const RootLayout* GetComputeRootLayout(const ComputePSOConfig& psoConfig);
    // GraphicsPSO取得
    ID3D12PipelineState* GetGraphicsPipelineState(const GraphicsPSOConfig& psoConfig, const RootLayout& layout);
    // ComputePSO取得
    ID3D12PipelineState* GetComputePipelineState(const ComputePSOConfig& psoConfig, const RootLayout& layout);
    // シェーダーBlobの取得
    Microsoft::WRL::ComPtr<IDxcBlob> GetShaderBlob(const wchar_t* path, const wchar_t* target);

private:

    // シェーダーのルートパラメータ取得
    const std::vector<RootParam>& GetShaderRootParams(const wchar_t* path, const wchar_t* target, ShaderType shaderType);
    // ルートレイアウトを組み立てる
    RootLayout BuildRootLayout(std::vector<RootParam> params, size_t shaderHash);
    // ルートシグネチャの取得
    ID3D12RootSignature* GetRootSignature(const std::vector<RootParam>& params);
    // ルートシグネチャ生成
    Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSignature(const std::vector<RootParam>& params);
    // GraphicsPSO生成
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipelineState(const GraphicsPSOConfig& cfg, const RootLayout& layout);
    // ComputePSO生成
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateComputePipelineState(const ComputePSOConfig& cfg, const RootLayout& layout);
    // シェーダーコンパイル
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);
	// シェーダーの情報を取得
    ShaderCacheEntry& GetShaderEntry(const wchar_t* path, const wchar_t* target);


    // シェーダーキャッシュ
    std::unordered_map<std::wstring, ShaderCacheEntry> shaderCache_;
    // ルートレイアウトキャッシュ(キーはシェーダーパスの組み合わせ)。
    std::unordered_map<std::string, RootLayout> graphicsLayoutCache_;
    std::unordered_map<std::string, RootLayout> computeLayoutCache_;
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
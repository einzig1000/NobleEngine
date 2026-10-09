#pragma once
#include <cstdint>
#include <d3d12.h>
#include <string>
#include <vector>
#include <unordered_map>

enum class ShaderType
{
	None,
    VertexShader,
    PixelShader,
    ComputeShader,
	MeshShader,
	AmplificationShader,
};

enum class ParamType
{
	None,
	CBV,
	SRV,
	UAV,
};

struct InputElement
{
    std::string semanticName;
	UINT semanticIndex;
    D3D12_INPUT_ELEMENT_DESC desc;
};

// ルートパラメータ1個分の「形」。シェーダーのリフレクションで決まり、同じシェーダーを使うオブジェクト全員で共有する
struct RootParam
{
    ParamType paramType = ParamType::None;
    ShaderType shaderType = ShaderType::None;
    uint32_t key = 0;           // "b0", "b1" など
    uint32_t registerSpace = 0; // space0, space1 など。2Dテクスチャバインドレスとddsテクスチャバインドレスで区別
    uint32_t hash = 0;          // paramType･shaderType･key･registerSpace のハッシュ

    // CBuffer用
    uint32_t sizeBytes = 0;     // 自身のサイズ。CBuffer用ストレージ内でどれだけのサイズが必要か。

    // SRV用。Bindlessテクスチャ配列(Texture2D tex[] のようにサイズ指定なし)ならtrue
    bool isUnbounded = false;

    static uint32_t MakeHash(ParamType paramType, ShaderType shaderType, uint32_t key, uint32_t registerSpace)
    {
        uint32_t h = 0;

        h ^= static_cast<uint32_t>(paramType);
        h *= 0x85ebca6b;

        h ^= static_cast<uint32_t>(shaderType);
        h *= 0x85ebca6b;

        h ^= key;
        h *= 0x85ebca6b;

        h ^= registerSpace;
        h *= 0x85ebca6b;

        h ^= h >> 16;
        h *= 0xc2b2ae35;
        h ^= h >> 16;

        return h;
    }

    void ComputeHash()
    {
        hash = MakeHash(paramType, shaderType, key, registerSpace);
    }
};

// ルートパラメータ1個分の「値」。オブジェクトごとに持ち、RootLayout::params と同じ並びで使う
struct RootParamValue
{
    D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = 0; // CBV用:このフレームに書き込み済みのCB GPUアドレス
    uint32_t allocIndex = UINT32_MAX;         // SRV/UAV用:Allocation.index
};

// シェーダーの組み合わせ1つ分のルートレイアウト。PipelineStateManagerが所有し、オブジェクトはポインタで参照する
struct RootLayout
{
    std::vector<RootParam> params;
    // RootParam::hash → params の添字
    std::unordered_map<uint32_t, size_t> hashToIndex;
    // シェーダーの組み合わせのハッシュ。PSOキャッシュのキーに使う
    size_t shaderHash = 0;
    // 作成済みのルートシグネチャ(所有はPipelineStateManagerのキャッシュ側)
    ID3D12RootSignature* rootSignature = nullptr;

    // 見つからなければ -1
    int32_t Find(ParamType paramType, ShaderType shaderType, uint32_t key, uint32_t registerSpace) const
    {
        const auto it = hashToIndex.find(RootParam::MakeHash(paramType, shaderType, key, registerSpace));
        if (it == hashToIndex.end()) return -1;
        return static_cast<int32_t>(it->second);
    }
};

// ブレンドステートの種類
enum class BlendStateID : uint8_t
{
    Opaque,
	Alpha,
    Normal,
    Normal2,
    Add,
    Sub,
    Mul,
    Screen,
};

// 深度ステンシルの種類
enum class DepthStencilID : uint8_t
{
    Default,      // depth write/test
    TestOnly,     // depth test only (write off)
    Disable,      // depth off
};

// ラスタライザの種類
enum class RasterizerID : uint8_t
{
    Solid_BackCull,      // Fill: Solid, Cull: Back
    Solid_FrontCull,     // Fill: Solid, Cull: Front
    Solid_NoCull,        // Fill: Solid, Cull: None

    Wireframe_NoCull,    // Fill: Wireframe, Cull: None
    Wireframe_BackCull,  // Fill: Wireframe, Cull: Back
};

// DSVformatの種類
enum class DSVFormatID : uint8_t
{
	D24,
	Unknown,
};


struct GraphicsPSOConfig
{
    /// 頂点シェーダーファイル名
    std::string vs = "unknown";
    /// ピクセルシェーダーファイル名
    std::string ps = "unknown";
	/// メッシュシェーダーファイル名
	std::string ms = "unknown";
	/// アンプリフィケーションシェーダーファイル名
	std::string as = "unknown";
    /// ブレンドステートID
    BlendStateID blendID = BlendStateID::Normal;
    /// 深度ステンシルID
    DepthStencilID depthStencilID = DepthStencilID::Default;
    /// ラスタライザーID
    RasterizerID rasterizerID = RasterizerID::Solid_NoCull;
    /// プリミティブトポロジ
    D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	/// DSVフォーマットID
	DSVFormatID dsvFormatID = DSVFormatID::D24;
};

struct ComputePSOConfig
{
	/// コンピュートシェーダーファイル名
    std::string cs = "unknown";
};
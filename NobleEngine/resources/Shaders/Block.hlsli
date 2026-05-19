// Block.hlsli


struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float2 texcoord2 : TEXCOORD1;
    float3 normal : NORMAL0;
};
struct VertexShaderOutput
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
    float2 texcoord2 : TEXCOORD1;
    float3 normal : NORMAL0;
    uint instancedID : TEXCOORD2; // TEXCOORD2ではない
};
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct WorldMatrix
{
    float4x4 World;
};
struct ViewProjectionMatrix
{
    float4x4 ViewProjection;
};

struct AtlasInfo
{
    uint atlasCols;     // 1固定（縦に積む想定）
    uint atlasRows;     // ブロック種類数（行数）
    uint faceStrideX;   // 24 (1面の占有幅)
    uint faceStrideY;   // 24 (1面の占有高)
    
    uint innerSizeX;    // 24x24のブロックテクスチャのうち実際に描画に使う部分の幅      16固定
    uint innerSizeY;    // 24x24のブロックテクスチャのうち実際に描画に使う部分の高さ    16固定
    uint padX;          // 24x24のブロックテクスチャのうち描画に使わない部分の幅        4固定
    uint padY;          // 24x24のブロックテクスチャのうち描画に使わない部分の高さ      4固定

    uint facesPerBlock; // 6
    uint padding0;

    float2 invAtlasSize; // (1/atlasWidth, 1/atlasHeight)
    float2 padding1; // 16byte境界
};
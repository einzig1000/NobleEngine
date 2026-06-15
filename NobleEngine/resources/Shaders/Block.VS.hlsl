// Block.VS.hlsl

struct VSInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float2 texcoord2 : TEXCOORD1;
    float3 normal : NORMAL0;
};

struct VSOutput
{
    float4 position : SV_Position;
    nointerpolation uint instancedID : TEXCOORD0;
    nointerpolation uint baseTextureID : TEXCOORD1;
    nointerpolation uint breakTextureID : TEXCOORD2;
    float2 texcoord : TEXCOORD3;
    float2 texcoord2 : TEXCOORD4;
    float3 normal : TEXCOORD5;
    float4 color : COLOR0;
};

struct InstanceData
{
    // ワールド行列
    float4x4 World;
    // 色
    float4 Color;
    // ベーステクスチャID
    uint BaseTextureID;
    // 破壊テクスチャID
    uint BreakTextureID;
};

// 各インスタンスのワールド座標
StructuredBuffer<InstanceData> instanceData : register(t0);

// ビュー射影行列
cbuffer ViewProjection : register(b0)
{
    float4x4 viewProjection;
}

VSOutput main(VSInput input, uint instancedID : SV_InstanceID)
{
    VSOutput output;

	// 座標変換
    float4 worldPos = mul(input.position, instanceData[instancedID].World);
    output.position = mul(worldPos, viewProjection);
    // 欲しいタイル番号取得
    output.baseTextureID = instanceData[instancedID].BaseTextureID;
    output.breakTextureID = instanceData[instancedID].BreakTextureID;
	// UV計算
    output.texcoord = input.texcoord;
    output.texcoord2 = input.texcoord2;
	// 法線変換
    output.normal = normalize(mul(input.normal, (float3x3) instanceData[instancedID].World));
    // 色
    output.color = instanceData[instancedID].Color;
	// インスタンスID
	output.instancedID = instancedID;

    return output;
}
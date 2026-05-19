#include "Block.hlsli"

// サンプラー
SamplerState gSampler : register(s0);
// ベーステクスチャ(アトラス)
Texture2D<float4> gTexture0 : register(t3);
// 破壊テクスチャ(アトラス)
Texture2D<float4> gBreakTexture : register(t4);
// インスタンスごとの色
StructuredBuffer<float4> gColor0 : register(t5);


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float4 baseColor = gTexture0.Sample(gSampler, input.texcoord);
    float4 breakColor = gBreakTexture.Sample(gSampler, input.texcoord2);

    float useBreak = step(1e-5, breakColor.a);
    float4 blended = lerp(baseColor, breakColor, useBreak);

    output.color = gColor0[input.instancedID] * blended;

    if (output.color.a <= 0.001) discard;
    return output;
}
struct PSInput
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

struct PSOutput
{
    float4 color : SV_Target;
};

// サンプラー
SamplerState gSampler : register(s0);
// テクスチャ
Texture2D<float4> textures[] : register(t0);

PSOutput main(PSInput input)
{
    PSOutput output;

    float4 baseColor = textures[input.baseTextureID].Sample(gSampler, input.texcoord);
    float4 breakColor = textures[input.breakTextureID].Sample(gSampler, input.texcoord2);

    float useBreak = step(1e-5, breakColor.a);
    float4 blended = lerp(baseColor, breakColor, useBreak);

    output.color = input.color * blended;

    if (output.color.a <= 0.001)
        discard;
    return output;
}
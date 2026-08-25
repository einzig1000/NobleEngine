struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
};

cbuffer PSConstants : register(b0)
{
    int atlasTextureIndex;
    float3 pad0;
};

Texture2D<float4> textures[] : register(t0);
SamplerState gSampler : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    float alpha = textures[atlasTextureIndex].Sample(gSampler, input.texcoord).r;
    return float4(input.color.rgb, input.color.a * alpha);
}
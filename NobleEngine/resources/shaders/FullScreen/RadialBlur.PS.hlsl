
struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

cbuffer TextureIndex : register(b0)
{
    int textureIndex;
};

Texture2D<float4> textures[] : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    const float2 kCenter = float2(0.5, 0.5);
    const int kNumSamples = 10;
    const float kBlurWidth = 0.01f;
    
    float2 direction = input.TexCoord - kCenter;
    float3 outputColor = float3(0.0, 0.0, 0.0);
    for (int i = 0; i < kNumSamples; ++i)
    {
        float2 texcoord = input.TexCoord + direction * kBlurWidth * float(i);
        outputColor.rgb += textures[textureIndex].Sample(gSampler, texcoord).rgb;
    }
    outputColor *= rcp(kNumSamples);
    
    PSOutput output;
    output.Color = float4(outputColor, textures[textureIndex].Sample(gSampler, input.TexCoord).a);
    return output;
}

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

cbuffer MaskTextureIndex : register(b1)
{
    int maskTextureIndex;
};

// 閾値
cbuffer Threshold : register(b2)
{
    float threshold;
};

cbuffer EdgeColor : register(b3)
{
    float3 edgeColor;
};

Texture2D<float4> textures[] : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput output;
    float mask = textures[maskTextureIndex].Sample(gSampler, input.TexCoord).r;
    if (mask < threshold)
    {
        discard;
    }
    output.Color = textures[textureIndex].Sample(gSampler, input.TexCoord);
    
    float edge = 1.0f - smoothstep(threshold, threshold + 0.5f, mask);
    
    output.Color.rgb += edge * edgeColor;
    
    
    return output;
}
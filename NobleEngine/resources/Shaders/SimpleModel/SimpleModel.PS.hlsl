// SimpleModel.PS.hlsl
struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

cbuffer ColorBuffer : register(b0)
{
    float4 color;
}

cbuffer TextureIndex : register(b1)
{
    int textureIndex;
};

Texture2D<float4> textures[] : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput output;
    float4 texColor = textures[textureIndex].Sample(gSampler, input.texCoord);
    output.color = color * texColor;
    if (output.color.a < 0.1f)
    {
        discard;
    }
    return output;
}


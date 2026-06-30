
struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

cbuffer Time : register(b0)
{
    float time;
};

float rand3dTo1d(float3 value)
{
    float3 smallValue = sin(value);
    float random = dot(smallValue, float3(12.9898, 78.233, 37.719));
    random = frac(sin(random) * 143758.5453);
    return random;
}

float rand2dTo1d(float2 value)
{
    float2 smallValue = sin(value);
    float random = dot(smallValue, float2(12.9898, 78.233));
    random = frac(sin(random) * 143758.5453);
    return random;
}

PSOutput main(PSInput input)
{
    PSOutput output;
    float rand = rand2dTo1d(input.TexCoord * time);
    output.Color = float4(rand, rand, rand, 1.0f);
    return output;
}
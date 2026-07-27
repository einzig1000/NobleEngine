#include "../Utilities/Utilities.hlsli"

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


PSOutput main(PSInput input)
{
    PSOutput output;
    float rand = rand2dTo1d(input.TexCoord * time);
    output.Color = float4(rand, rand, rand, 1.0f);
    return output;
}
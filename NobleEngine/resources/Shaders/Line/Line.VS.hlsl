// Line.VS.hlsl

struct VSInput
{
    float4 position : POSITION0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

cbuffer ViewProjection : register(b0)
{
    float4x4 vp;
}

VSOutput main(VSInput input)
{
    VSOutput output;
    
    output.position = mul(input.position, vp);

    return output;
}
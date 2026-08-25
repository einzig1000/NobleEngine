// Line.VS.hlsl

struct VSInput
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

cbuffer ViewProjection : register(b0)
{
    float4x4 vp;
}

VSOutput main(VSInput input)
{
    VSOutput output;
    
    output.position = mul(input.position, vp);
    output.color = input.color;

    return output;
}
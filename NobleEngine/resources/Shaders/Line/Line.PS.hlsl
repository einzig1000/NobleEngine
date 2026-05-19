// Line.PS.hlsl
struct PSInput
{
    float4 position : SV_POSITION;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

cbuffer ColorBuffer : register(b0)
{
    float4 color;
}

PSOutput main(PSInput input)
{
    PSOutput output;
    
    output.color = color;
    
    return output;
}
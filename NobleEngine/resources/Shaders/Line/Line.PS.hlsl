// Line.PS.hlsl
struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

PSOutput main(PSInput input)
{
    PSOutput output;
    
    output.color = input.color;
    
    return output;
}
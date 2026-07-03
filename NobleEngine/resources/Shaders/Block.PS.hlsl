struct PSInput
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR0;
};

struct PSOutput
{
    float4 color : SV_Target;
};

PSOutput main(PSInput input)
{
    PSOutput output;
    
    output.color = input.color;

    if (output.color.a <= 0.001) discard;
    return output;
}

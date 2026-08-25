struct PSInput
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

struct PSOutput
{
    float4 color : SV_Target;
};

PSOutput main(PSInput input)
{
    PSOutput output;

    float edgeDist = min(min(input.uv.x, 1.0f - input.uv.x), min(input.uv.y, 1.0f - input.uv.y));
    float3 rgb = (edgeDist < 0.01f) ? input.color.rgb * 0.5f : input.color.rgb;

    output.color = float4(rgb, input.color.a);

    if (output.color.a <= 0.001) discard;
    return output;
}

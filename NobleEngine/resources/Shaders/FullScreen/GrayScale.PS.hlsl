
struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput output;
    output.Color = gTexture.Sample(gSampler, input.TexCoord);
    float value = dot(output.Color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
    output.Color.rgb = float3(value, value, value);
    return output;
}
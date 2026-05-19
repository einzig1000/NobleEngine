
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
    return output;
}
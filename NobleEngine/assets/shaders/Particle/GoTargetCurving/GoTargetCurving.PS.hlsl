struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
    nointerpolation int textureID : TEXTUREID0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    Texture2D<float4> particleTexture = ResourceDescriptorHeap[NonUniformResourceIndex(input.textureID)];

    PSOutput output;
    output.color = input.color * particleTexture.Sample(gSampler, input.texcoord);
    if (output.color.a < 0.01f)
    {
        discard;
    }
    return output;
}
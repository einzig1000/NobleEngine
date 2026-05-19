// SkyBox.PS

struct PSInput
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

cbuffer TextureIndex : register(b0)
{
    int textureIndex;
};

TextureCube<float4> textures[] : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput output;
    float4 textureColor = textures[textureIndex].Sample(gSampler, input.texCoord);
    output.color = textureColor;
    return output;
}
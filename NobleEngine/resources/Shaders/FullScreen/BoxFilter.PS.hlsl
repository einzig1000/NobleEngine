

static const float2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }
};

static const float kKernel3x3[3][3] =
{
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f }
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

cbuffer TextureIndex : register(b1)
{
    int textureIndex;
};

Texture2D<float4> textures[] : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    float2 size;
    textures[textureIndex].GetDimensions(size.x, size.y);
    float2 uvStepSize = rcp(size);
    
    PSOutput output;
    output.Color.rgb = float3(0.0f, 0.0f, 0.0f);
    output.Color.a = 1.0f;
    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            float2 texcoord = input.TexCoord + kIndex3x3[y][x] * uvStepSize;
            float3 fetchColor = textures[textureIndex].Sample(gSampler, texcoord).rgb;
            output.Color.rgb += fetchColor * kKernel3x3[y][x];
        }
    }
    
    return output;
}

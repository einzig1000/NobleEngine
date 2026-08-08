

static const float2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }
};

static const float kPrewittHorizontalKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f }
};

static const float kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f }
};

float Luminance(float3 color)
{
    return dot(color, float3(0.2125f, 0.7154f, 0.0721f));
}

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

cbuffer TextureIndex : register(b0)
{
    int textureIndex;
};

cbuffer DepthTextureIndex : register(b1)
{
    int depthTextureIndex;
};

cbuffer ProjectionInverse : register(b2)
{
    float4x4 projectionInverse;
};

Texture2D<float4> textures[] : register(t0);
SamplerState gSampler : register(s0);
SamplerState gSamplerPoint : register(s1);

PSOutput main(PSInput input)
{
    float2 size;
    textures[textureIndex].GetDimensions(size.x, size.y);
    float2 uvStepSize = rcp(size);
    
    
    float2 difference = float2(0.0f, 0.0f);
    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            float2 texcoord = input.TexCoord + kIndex3x3[y][x] * uvStepSize;
            float ndcDepth = textures[depthTextureIndex].Sample(gSamplerPoint, texcoord).r;
            float4 viewSpace = mul(float4(0.0f, 0.0f, ndcDepth, 1.0f), projectionInverse);
            float viewZ = viewSpace.z * rcp(viewSpace.w);
            difference.x += viewZ * kPrewittHorizontalKernel[y][x];
            difference.y += viewZ * kPrewittVerticalKernel[y][x];
        }
    }
    
    float weight = length(difference);
    weight = saturate(weight);
    PSOutput output;
    float4 color = textures[textureIndex].Sample(gSampler, input.TexCoord);
    output.Color.rgb = (1.0f - weight) * color.rgb;
    output.Color.a = color.a;
    
    return output;
}

// GaussianFilter.PS

static const float PI = 3.141592653589f;

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

static const float2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }
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

cbuffer TextureIndex : register(b0)
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
    float weight = 0.0f;
    float kernel3x3[3][3];
    for (int y1 = 0; y1 < 3; ++y1)
    {
        for (int x1 = 0; x1 < 3; ++x1)
        {
            kernel3x3[y1][x1] = gauss(kIndex3x3[y1][x1].x, kIndex3x3[y1][x1].y, 2.0f);
            weight += kernel3x3[y1][x1];
        }
    }
    for (int y2 = 0; y2 < 3; ++y2)
    {
        for (int x2 = 0; x2 < 3; ++x2)
        {
            float2 texcoord = input.TexCoord + kIndex3x3[y2][x2] * uvStepSize;
            float3 fetchColor = textures[textureIndex].Sample(gSampler, texcoord).rgb;
            output.Color.rgb += fetchColor * kernel3x3[y2][x2];
        }
    }
    
    output.Color.rgb *= rcp(weight);
    
    return output;
}

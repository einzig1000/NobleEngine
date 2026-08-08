static const float PI = 3.141592653589f;

float gauss1D(float x, float sigma)
{
    float exponent = -(x * x) * rcp(2.0f * sigma * sigma);
    float denominator = sqrt(2.0f * PI) * sigma;
    return exp(exponent) * rcp(denominator);
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
cbuffer radius : register(b1)
{
    int radius; // ぼかし半径
}
cbuffer TexelSize : register(b2)
{
    float2 texelSize; // テクセルサイズ
}

Texture2D<float4> textures[] : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    float sigma = max(float(radius) / 3.0f, 0.0001f);

    float4 sum = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float weightSum = 0.0f;

    [loop]
    for (int i = -radius; i <= radius; ++i)
    {
        float w = gauss1D(float(i), sigma);
        float2 uv = input.TexCoord + float2(0.0f, texelSize.y * i);
        sum += textures[textureIndex].Sample(gSampler, uv) * w;
        weightSum += w;
    }

    PSOutput output;
    output.Color = sum * rcp(weightSum);
    return output;
}

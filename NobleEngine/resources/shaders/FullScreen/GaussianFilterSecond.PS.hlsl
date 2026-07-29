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

Texture2D<float4> textures[] : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    float2 size;
    textures[textureIndex].GetDimensions(size.x, size.y);
    float2 texel = rcp(size);

    // radiusからsigmaを自動算出（n/3はカーネル端まで滑らかに減衰させる経験則）
    float sigma = max(float(radius) / 3.0f, 0.0001f);

    float3 sum = float3(0.0f, 0.0f, 0.0f);
    float weightSum = 0.0f;

    [loop]
    for (int i = -radius; i <= radius; ++i)
    {
        float w = gauss1D(float(i), sigma);
        float2 uv = input.TexCoord + float2(0.0f, texel.y * i);
        sum += textures[textureIndex].Sample(gSampler, uv).rgb * w;
        weightSum += w;
    }

    PSOutput output;
    output.Color.rgb = sum * rcp(weightSum);
    output.Color.a = 1.0f;
    return output;
}

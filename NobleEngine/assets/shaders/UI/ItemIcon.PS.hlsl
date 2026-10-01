struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

cbuffer ColorBuffer : register(b0)
{
    float4 color;
}

cbuffer TextureIndex : register(b1)
{
    int textureIndex;
};

SamplerState gSamplerPoint : register(s1);

static const float kShadeX = 0.60f; // ±X
static const float kShadeTop = 1.00f; // +Y
static const float kShadeBottom = 0.50f; // -Y
static const float kShadeZ = 0.80f; // ±Z

float FaceShade(float3 normal)
{
    float3 a = abs(normal);
    if (a.y >= a.x && a.y >= a.z)
    {
        return (normal.y >= 0.0f) ? kShadeTop : kShadeBottom;
    }
    return (a.x >= a.z) ? kShadeX : kShadeZ;
}

PSOutput main(PSInput input)
{
    Texture2D<float4> targetTexture = ResourceDescriptorHeap[textureIndex];

    PSOutput output;
    output.color = color * targetTexture.Sample(gSamplerPoint, input.texCoord);
    output.color.rgb *= FaceShade(input.normal);
    if (output.color.a < 0.1f)
    {
        discard;
    }
    return output;
}

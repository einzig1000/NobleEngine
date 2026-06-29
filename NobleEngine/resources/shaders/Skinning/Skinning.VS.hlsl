// SimpleModel.VS.hlsl
struct VSInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 weight : WEIGHT0;
    int4 index : INDEX0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct Well
{
    float4x4 skeletonSpaceMatrix;
    float4x4 skeletonSpaceInverseTransposeMatrix;
};

struct Skinned
{
    float4 position;
    float3 normal;
};

cbuffer WVP : register(b0)
{
    float4x4 wvp;
}

cbuffer World : register(b1)
{
    float4x4 world;
}

StructuredBuffer<Well> gMatrixPalette : register(t0);

Skinned Skinning(VSInput input)
{
    Skinned skinned;
    skinned.position = float4(0.0f, 0.0f, 0.0f, 0.0f);
    skinned.normal = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; ++i)
    {
        if (input.weight[i] > 0.0f)
        {
            Well well = gMatrixPalette[input.index[i]];
            skinned.position += mul(input.position, well.skeletonSpaceMatrix) * input.weight[i];
            skinned.normal += mul(input.normal, (float3x3) well.skeletonSpaceInverseTransposeMatrix) * input.weight[i];
        }
    }
    return skinned;
}

VSOutput main(VSInput input)
{
    VSOutput output;
    Skinned skinned = Skinning(input);
    output.position = mul(skinned.position, wvp);
    output.texCoord = input.texCoord;
    float3 worldNormal = mul(skinned.normal, (float3x3) world);
    output.normal = normalize(worldNormal);
    return output;
}

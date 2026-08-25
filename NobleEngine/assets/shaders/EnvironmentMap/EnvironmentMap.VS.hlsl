// EnvironmentMap.VS.hlsl
struct VSInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};
struct VSOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float2 texCoord : TEXCOORD2;
};

cbuffer WVP : register(b0)
{
    float4x4 wvp;
}

cbuffer World : register(b1)
{
    float4x4 world;
}

VSOutput main(VSInput input)
{
    VSOutput output;
    output.position = mul(input.position, wvp);
    float4 worldPos = mul(input.position, world);
    output.worldPosition = worldPos.xyz;
    output.texCoord = input.texCoord;
    float3 worldNormal = mul(input.normal, (float3x3) world);
    output.normal = normalize(worldNormal);
    return output;
}

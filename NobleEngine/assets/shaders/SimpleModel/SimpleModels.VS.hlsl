// SimpleModels.VS.hlsl
struct VSInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    uint instanceID : SV_InstanceID;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    uint instanceID : SV_InstanceID;
};

cbuffer ViewProjectionMatrix : register(b0)
{
    float4x4 vp;
}

StructuredBuffer<float4x4> worldList : register(t0);

VSOutput main(VSInput input)
{
    VSOutput output;
    float4x4 world = worldList[input.instanceID];
    float4 worldPos = mul(input.position, world);
    output.position = mul(worldPos, vp);
    float3 worldNormal = mul(input.normal, (float3x3) world);
    output.normal = normalize(worldNormal);
    output.texCoord = input.texCoord;
    output.instanceID = input.instanceID;
    return output;
}

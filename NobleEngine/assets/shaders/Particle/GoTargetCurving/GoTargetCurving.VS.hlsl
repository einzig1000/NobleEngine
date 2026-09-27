struct VSInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
    nointerpolation int textureID : TEXTUREID0;
};

// GoTargetCurving::Instanceと同じ並び
struct Instance
{
    float3 position;
    float scale;
    float4 color;
    int textureID;
};

cbuffer PerView : register(b0)
{
    float4x4 viewProjection;
    float4x4 billboardMatrix;
};

StructuredBuffer<Instance> gInstances : register(t0);

VSOutput main(VSInput input, uint instanceId : SV_InstanceID)
{
    Instance instance = gInstances[instanceId];
    
    float4x4 worldMatrix = billboardMatrix;
    worldMatrix[0] *= instance.scale;
    worldMatrix[1] *= instance.scale;
    worldMatrix[2] *= instance.scale;
    worldMatrix[3].xyz = instance.position;

    VSOutput output;
    output.position = mul(input.position, mul(worldMatrix, viewProjection));
    output.texcoord = input.texcoord;
    output.color = instance.color;
    output.textureID = instance.textureID;
    return output;
}
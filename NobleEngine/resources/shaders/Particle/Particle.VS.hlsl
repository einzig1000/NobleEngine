// ParticleBillboardVS.hlsl
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
};

struct Particle
{
    float3 translate;
    float3 scale;
    float lifeTime;
    float3 velocity;
    float currentTime;
    float4 color;
};

cbuffer PerView : register(b0)
{
    float4x4 viewProjection;
    float4x4 billboardMatrix;
};

StructuredBuffer<Particle> gParticles : register(t0);

VSOutput main(VSInput input, uint instanceId : SV_InstanceID)
{
    VSOutput output;

    // インスタンスIDに対応するパーティクル情報を取得
    Particle particle = gParticles[instanceId];

    // ビルボード行列をコピーしてスケール・平行移動を適用
    float4x4 worldMatrix = billboardMatrix;
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;
    output.position = mul(input.position, mul(worldMatrix, viewProjection));
    output.texcoord = input.texcoord;
    output.color = particle.color;

    return output;
}

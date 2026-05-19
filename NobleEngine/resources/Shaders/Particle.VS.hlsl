// Particle.VS.hlsl

#include "Particle.hlsli"

// TransformationMatrix: レジスタ t1 に設定
StructuredBuffer<WorldMatrix> gWorldMatrix : register(t1);

ConstantBuffer<ViewProjectionMatrix> gViewProjection : register(b1);

VertexShaderOutput main(VertexShaderInput input, uint32_t instancedID : SV_InstanceID)
{
    VertexShaderOutput output;

    float32_t4 worldPos = mul(input.position, gWorldMatrix[instancedID].World);
    output.position = mul(worldPos, gViewProjection.ViewProjection);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3)gWorldMatrix[instancedID].World));
    return output;
}
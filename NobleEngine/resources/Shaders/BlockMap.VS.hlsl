// BlockMap.VS.hlsl

#include "BlockMap.hlsli"

StructuredBuffer<TransformationMatrix> gTransformationMatrix : register(t1);

VertexShaderOutput main(VertexShaderInput input, uint32_t instancedID : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix[instancedID].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrix[instancedID].World));
    return output;
}
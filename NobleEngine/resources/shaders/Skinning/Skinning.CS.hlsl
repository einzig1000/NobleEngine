// Skinning.CS.hlsl

struct Vertex
{
    float4 position;
    float2 texcoord;
    float3 normal;
};

struct VertexInfluence
{
    float4 weight;
    int4 index;
};

struct SkinningInformation
{
    // 頂点数
    uint numVertices;
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

// SkinningObject3d.VS.hlslで作ったものと同じPalette
StructuredBuffer<Well> gMatrixPalette : register(t0);
// VertexBufferViewのstream0として利用していた入力頂点
StructuredBuffer<Vertex> gInputVertices : register(t1);
// VertexBufferViewのstream1として利用していたスキンインフルエンス
StructuredBuffer<VertexInfluence> gInfluences : register(t2);
// Skinning計算後の頂点データ。SkinnedVertex
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);
// Skinningに関するちょっとした情報
ConstantBuffer<SkinningInformation> gSkinningInformation : register(b0);


Skinned Skinning(VertexInfluence influence, Vertex vertex)
{
    Skinned skinned;
    skinned.position = float4(0.0f, 0.0f, 0.0f, 0.0f);
    skinned.normal = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; ++i)
    {
        if (influence.weight[i] > 0.0f)
        {
            Well well = gMatrixPalette[influence.index[i]];
            skinned.position += mul(vertex.position, well.skeletonSpaceMatrix) * influence.weight[i];
            skinned.normal += mul(vertex.normal, (float3x3) well.skeletonSpaceInverseTransposeMatrix) * influence.weight[i];
        }
    }
    return skinned;
}

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint vertexIndex = DTid.x;
    if (vertexIndex < gSkinningInformation.numVertices)
    {
        Vertex input = gInputVertices[vertexIndex];
        VertexInfluence influence = gInfluences[vertexIndex];
        
        Skinned skinned = Skinning(influence, input);
       
        Vertex output;
        output.position = skinned.position;
        output.texcoord = input.texcoord;
        output.normal = normalize(skinned.normal);
        
        gOutputVertices[vertexIndex] = output;
    }
}
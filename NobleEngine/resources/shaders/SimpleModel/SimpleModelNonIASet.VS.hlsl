struct Vertex
{
    float4 position;
    float2 texCoord;
    float3 normal;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

cbuffer WVP : register(b0)
{
    float4x4 wvp;
}

cbuffer World : register(b1)
{
    float4x4 world;
}

StructuredBuffer<Vertex> gVertices : register(t0);

VSOutput main(uint vertexID : SV_VertexID)
{
    VSOutput output;
    Vertex vertex = gVertices[vertexID];
    output.position = mul(vertex.position, wvp);
    output.texCoord = vertex.texCoord;
    float3 worldNormal = mul(vertex.normal, (float3x3) world);
    output.normal = normalize(worldNormal);
    return output;
}

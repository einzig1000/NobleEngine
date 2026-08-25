// Line.MS.hlsl

struct MSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

struct LineVertex
{
    float3 position;
};

cbuffer ViewProjection : register(b0)
{
    float4x4 vp;
}

cbuffer LineInfo : register(b1)
{
    // 色
    float4 color;
    // 頂点数
    uint vertexCount;
    // ライン配列のヒープスロット
    uint lineArrayHeapSlot;
}

static const uint MAX_VERTS = 128;

[NumThreads(MAX_VERTS, 1, 1)]
[OutputTopology("line")]
void main(
    uint gtid : SV_GroupThreadID,
    out vertices MSOutput verts[MAX_VERTS],
    out indices uint2 lines[MAX_VERTS - 1]
)
{
    // ライン配列
    StructuredBuffer<LineVertex> gVertices = ResourceDescriptorHeap[lineArrayHeapSlot];
    
    const uint count = min(vertexCount, MAX_VERTS);
    const uint primCount = count > 0 ? count - 1 : 0;
    SetMeshOutputCounts(count, primCount);

    if (gtid < count)
    {
        LineVertex v = gVertices[gtid];
        verts[gtid].position = mul(float4(v.position, 1.0f), vp);
        verts[gtid].color = color;
    }

    if (gtid < primCount)
    {
        lines[gtid] = uint2(gtid, gtid + 1);
    }
}
#include "../Utilities/Utilities.hlsli"

struct MSOutput
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR0;
};

struct VertexData
{
    float4 position;
    float3 normal;
    float2 uv;
};

struct Meshlet
{
    uint vertexOffset; // uniqueVertexIndices の開始位置
    uint primitiveOffset; // primitiveIndices の開始位置
    uint vertexCount; // このメッシュレットが持つ頂点数 (最大64)
    uint primitiveCount; // このメッシュレットが持つポリゴン数 (最大126)
};

cbuffer ModelInfo : register(b0)
{
    uint vertexSrvIndex;
    uint meshletSrvIndex;
    uint uniqueIndexSrvIndex;
    uint primIndexSrvIndex;
};

cbuffer WVP : register(b1)
{
    float4x4 wvp;
}

StructuredBuffer<uint> vertexColor : register(t0);



// 出力トポロジは三角形。スレッド数は最大128(頂点最大64, プリミティブ最大126をカバー)
[outputtopology("triangle")]
[numthreads(128, 1, 1)]
void main(
    uint gtid : SV_GroupIndex, // スレッドグループ内のローカルインデックス(0〜127)
    uint gid : SV_GroupID, // スレッドグループのID ＝ メッシュレットのインデックス
    out indices uint3 tris[126],
    out vertices MSOutput verts[64]
)
{
    StructuredBuffer<Meshlet> myMeshlets = ResourceDescriptorHeap[meshletSrvIndex];
    StructuredBuffer<VertexData> myVertices = ResourceDescriptorHeap[vertexSrvIndex];
    StructuredBuffer<uint> myUniqueVertexIndices = ResourceDescriptorHeap[uniqueIndexSrvIndex];
    StructuredBuffer<uint> myPrimitiveIndices = ResourceDescriptorHeap[primIndexSrvIndex];
    
    // 自分が担当するメッシュレットを取得
    Meshlet m = myMeshlets[gid];

    // 出力する頂点数とプリミティブ数(ポリゴン数)を宣言
    SetMeshOutputCounts(m.vertexCount, m.primitiveCount);

    // 頂点データの計算と出力
    if (gtid < m.vertexCount)
    {
        // ユニークインデックスを経由して、大元の頂点バッファから実際のインデックスを取得
        uint vertexIndex = myUniqueVertexIndices[m.vertexOffset + gtid];
        VertexData v = myVertices[vertexIndex];
        uint color = vertexColor[vertexIndex];

        // 座標変換などの処理
        MSOutput vOut;
        vOut.position = mul(v.position, wvp);
        vOut.normal = v.normal;
        float r = ((color >> 24) & 0xFF) / 255.0f + (rand4dTo1d(v.position) * 0.1);
        float g = ((color >> 16) & 0xFF) / 255.0f + (rand4dTo1d(v.position) * 0.1);
        float b = ((color >> 8) & 0xFF) / 255.0f + (rand4dTo1d(v.position) * 0.1);
        float a = (color & 0xFF) / 255.0f;
        vOut.color = float4(r, g, b, a);

        // 頂点配列に出力
        verts[gtid] = vOut;
    }

    // プリミティブ(インデックス)データの展開と出力
    if (gtid < m.primitiveCount)
    {
        // 1つのuint(32bit)の中に、10bit x 3 でパックされたインデックスを取り出す
        uint packedIndices = myPrimitiveIndices[m.primitiveOffset + gtid];

        // ビットシフトとマスク(0x3FF = 1023)で分解
        uint idx0 = packedIndices & 0x3FF;
        uint idx1 = (packedIndices >> 10) & 0x3FF;
        uint idx2 = (packedIndices >> 20) & 0x3FF;

        // 三角形配列に出力
        tris[gtid] = uint3(idx0, idx1, idx2);
    }
}
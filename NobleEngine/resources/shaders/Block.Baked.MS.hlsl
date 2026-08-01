#include "Utilities/Utilities.hlsli"

// CSで焼き込み済みの露出面リストを読んで頂点展開するだけのメッシュシェーダー。
// 隣接ブロック参照・露出判定・prefix sumは一切行わない(CS側で完了済みのものを読むだけ)。

static const float3 kCubeCorners[8] =
{
    float3(0, 0, 0), float3(1, 0, 0), float3(0, 1, 0), float3(1, 1, 0),
    float3(0, 0, 1), float3(1, 0, 1), float3(0, 1, 1), float3(1, 1, 1),
};

static const uint4 kFaceCorners[6] =
{
    uint4(1, 3, 7, 5), uint4(0, 4, 6, 2),
    uint4(2, 6, 7, 3), uint4(0, 1, 5, 4),
    uint4(5, 7, 6, 4), uint4(0, 2, 3, 1),
};

static const float3 kFaceNormal[6] =
{
    float3(1, 0, 0), float3(-1, 0, 0),
    float3(0, 1, 0), float3(0, -1, 0),
    float3(0, 0, 1), float3(0, 0, -1),
};

struct MSOutput
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR0;
};

struct BakedFace
{
    uint packedVoxelPos;
    uint faceIndex;
    uint blockId;
};

static const uint kMaxFacesPerGroup = 48;

cbuffer ChunkInfo : register(b0)
{
    float3 chunkWorldOrigin;
    float blockSize;
};

cbuffer WVP : register(b1)
{
    float4x4 wvp;
}

cbuffer SrvIndex : register(b2)
{
    // ブロック情報テーブルのSRVスロット
    uint blockInfoTableSrvIndex;
    // 露出面リストのSRVスロット
    uint bakedFaceListSrvIndex;
    // グループごとの露出面数のSRVスロット
    uint bakedFaceCountPerGroupSrvIndex;
};

[outputtopology("triangle")]
[numthreads(48, 1, 1)]
void main(
    uint gtid : SV_GroupIndex,
    uint3 gid : SV_GroupID,
    out indices uint3 tris[96],
    out vertices MSOutput verts[192]
)
{
    // ブロック情報テーブル
    // x: 16進数color                  BlockInfo::color
    // y: 透過ブロックかどうか         BlockInfo::isTransparent
    // z, w: きっといつか使う
    StructuredBuffer<uint4> blockInfoTable = ResourceDescriptorHeap[blockInfoTableSrvIndex];
    
    StructuredBuffer<BakedFace> bakedFaces = ResourceDescriptorHeap[bakedFaceListSrvIndex];
    
    StructuredBuffer<uint> faceCountPerGroup = ResourceDescriptorHeap[bakedFaceCountPerGroupSrvIndex];
    
    uint faceCount = faceCountPerGroup[gid.x];
    SetMeshOutputCounts(faceCount * 4, faceCount * 2);

    if (gtid >= faceCount)
        return; // このグループに割り当てられた面がないスレッドは何もしない

    BakedFace face = bakedFaces[gid.x * kMaxFacesPerGroup + gtid];
    int3 voxelPos = int3(
        face.packedVoxelPos & 0xFF,
        (face.packedVoxelPos >> 8) & 0xFF,
        (face.packedVoxelPos >> 16) & 0xFF);

    float3 worldVoxelOrigin = chunkWorldOrigin + float3(voxelPos) * blockSize;
    float4 color = UnpackColor(blockInfoTable[face.blockId].x);
    color.x += rand3dTo1d(voxelPos) * 0.1;
    color.y += rand3dTo1d(voxelPos) * 0.1;
    color.z += rand3dTo1d(voxelPos) * 0.1;

    uint vBase = gtid * 4;
    uint pBase = gtid * 2;
    uint4 corners = kFaceCorners[face.faceIndex];

    [unroll]
    for (uint c = 0; c < 4; c++)
    {
        float3 worldPos = worldVoxelOrigin + kCubeCorners[corners[c]] * blockSize;
        MSOutput vOut;
        vOut.position = mul(float4(worldPos, 1.0f), wvp);
        vOut.normal = kFaceNormal[face.faceIndex];
        vOut.color = color;
        verts[vBase + c] = vOut;
    }

    tris[pBase + 0] = uint3(vBase, vBase + 2, vBase + 1);
    tris[pBase + 1] = uint3(vBase, vBase + 3, vBase + 2);
}
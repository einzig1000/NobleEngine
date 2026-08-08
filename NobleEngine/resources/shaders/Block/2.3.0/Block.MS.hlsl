#include "Block.hlsli"
#include "../../Utilities/Utilities.hlsli"

struct MSOutput
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR0;
};

cbuffer SrvIndex : register(b0)
{
    // ブロック情報テーブルのSRVスロット
    uint blockInfoTableSrvIndex;
    // 露出面リストのSRVスロット
    uint bakedFaceListSrvIndex;
    // グループごとの露出面数のSRVスロット
    uint bakedFaceCountPerGroupSrvIndex;
};

cbuffer ChunkInfo : register(b1)
{
    float3 chunkWorldOrigin;
    float blockSize;
};

cbuffer WVP : register(b2)
{
    float4x4 wvp;
}

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
    
    // 露出面リスト
    StructuredBuffer<BakedFace> bakedFaces = ResourceDescriptorHeap[bakedFaceListSrvIndex];
    
    // グループごとの露出面数
    StructuredBuffer<uint> faceCountPerGroup = ResourceDescriptorHeap[bakedFaceCountPerGroupSrvIndex];
    
    
    
    // このグループの露出面数取得
    uint faceCount = faceCountPerGroup[gid.x];
    
    // 面数頂点数セット
    SetMeshOutputCounts(faceCount * 4, faceCount * 2);

    // 露出面0なら帰宅
    if (gtid >= faceCount)
        return;

    // gid.x * kMaxFacesPerGroup + gtid で各ブロックのユニークなインデックスを取得(ハッシュみたいなもの)
    BakedFace face = bakedFaces[gid.x * kMaxFacesPerGroup + gtid];
    
    // このブロックの整数座標を取得
    int3 voxelPos = int3(
        face.packedVoxelPos & 0xFF,
        (face.packedVoxelPos >> 8) & 0xFF,
        (face.packedVoxelPos >> 16) & 0xFF);
    // このブロックの座標を計算
    float3 worldVoxelOrigin = chunkWorldOrigin + float3(voxelPos) * blockSize;
    // このブロックの色を取得
    float4 color = UnpackColor(blockInfoTable[face.blockId].x);
    color.x += rand3dTo1d(voxelPos) * 0.1;
    color.y += rand3dTo1d(voxelPos) * 0.1;
    color.z += rand3dTo1d(voxelPos) * 0.1;

    // 頂点配列のユニークなインデックスを計算
    uint vBase = gtid * 4;
    // 三角形配列のユニークなインデックスを計算
    uint pBase = gtid * 2;
    // この面の4頂点のインデックスを取得(立方体の8頂点を通し番号で見た時のインデックス)
    uint4 corners = kFaceInfos[face.faceIndex].faceCorners;

    [unroll]
    for (uint c = 0; c < 4; c++)
    {
        MSOutput vOut;
        // 頂点座標を計算
        float3 worldPos = worldVoxelOrigin + (kCubeCorners[corners[c]] * blockSize);
        vOut.position = mul(float4(worldPos, 1.0f), wvp);
        // 法線をセット
        vOut.normal = kFaceInfos[face.faceIndex].faceNormal;
        // 色をセット
        vOut.color = color;
        // 頂点配列に書き込み
        verts[vBase + c] = vOut;
    }

    // 三角形配列に書き込み
    tris[pBase + 0] = uint3(vBase, vBase + 2, vBase + 1);
    tris[pBase + 1] = uint3(vBase, vBase + 3, vBase + 2);
}
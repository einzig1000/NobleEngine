#include "../3.0.0/Block.hlsli"
#include "../../Utilities/Utilities.hlsli"

struct MSOutput
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

static const float2 kQuadUVs[4] =
{
    float2(0, 0), float2(1, 0), float2(1, 1), float2(0, 1)
};

struct Payload
{
    uint groupIndices[32];
};

cbuffer HeapSlot : register(b0)
{
    // ブロック情報テーブルのヒープスロット
    uint blockInfoTableHeapSlot;
    // 露出面リストのヒープスロット
    uint bakedFaceListHeapSlot;
    // グループごとの露出面数のヒープスロット
    uint bakedFaceCountPerGroupHeapSlot;
    // グループごとの露出面オフセットのヒープスロット
    uint bakedFaceGroupOffsetHeapSlot;
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

cbuffer PageInfo : register(b3)
{
    // このチャンクが保有しているページ番号一覧のヒープスロット
    uint myPagesHeapSlot;
    // 1ページの面数(48面)
    uint pageSize;
}

[outputtopology("triangle")]
[numthreads(48, 1, 1)]
void main(
    in payload Payload payload,
    uint threadID : SV_GroupIndex,
    uint3 groupID : SV_GroupID,
    out indices uint3 tris[96],
    out vertices MSOutput verts[192]
)
{
    // ブロック情報テーブル
    // x: 16進数color                  BlockInfo::color
    // y: 透過ブロックかどうか         BlockInfo::isTransparent
    // z, w: きっといつか使う
    StructuredBuffer<uint4> blockInfoTable = ResourceDescriptorHeap[blockInfoTableHeapSlot];
    
    // 露出面リスト
    StructuredBuffer<BakedFace> bakedFaces = ResourceDescriptorHeap[bakedFaceListHeapSlot];
    
    // グループごとの露出面数
    StructuredBuffer<uint> faceCountPerGroup = ResourceDescriptorHeap[bakedFaceCountPerGroupHeapSlot];
    
    // 自グループの面リストはどこから始まるかのオフセット
    StructuredBuffer<uint> faceGroupOffset = ResourceDescriptorHeap[bakedFaceGroupOffsetHeapSlot];
    
    // このチャンクが保有しているページ番号一覧
    StructuredBuffer<uint> myPages = ResourceDescriptorHeap[myPagesHeapSlot];
    
    
    uint groupIndex = payload.groupIndices[groupID.x];
    
    // このグループの露出面数取得
    uint faceCount = faceCountPerGroup[groupIndex];
    
    // 面数頂点数セット
    SetMeshOutputCounts(faceCount * 4, faceCount * 2);

    // 露出面0なら帰宅
    if (threadID >= faceCount) return;
    
    // チャンクローカルの線形インデックスを、ページ番号とページ内スロットに変換
    uint localIndex = faceGroupOffset[groupIndex] + threadID;
    uint pageLocalIndex = localIndex / pageSize;
    uint slotInPage = localIndex % pageSize;

    uint myPageCount, pageStride;
    myPages.GetDimensions(myPageCount, pageStride);
    
    if (pageLocalIndex >= myPageCount) return;
    
    uint globalPage = myPages[pageLocalIndex];
    uint globalSlot = globalPage * pageSize + slotInPage;

    // このスレッドの面情報を取得
    BakedFace face = bakedFaces[globalSlot];
    
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
    uint vBase = threadID * 4;
    // 三角形配列のユニークなインデックスを計算
    uint pBase = threadID * 2;
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
        // ローカルUVをセット(枠描画用)
        vOut.uv = kQuadUVs[c];
        // 頂点配列に書き込み
        verts[vBase + c] = vOut;
    }

    // 三角形配列に書き込み
    tris[pBase + 0] = uint3(vBase, vBase + 2, vBase + 1);
    tris[pBase + 1] = uint3(vBase, vBase + 3, vBase + 2);
}
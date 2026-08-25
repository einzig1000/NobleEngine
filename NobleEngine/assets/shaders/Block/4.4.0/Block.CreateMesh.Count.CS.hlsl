#include "../3.0.0/Block.hlsli"

cbuffer HeapSlot : register(b0)
{
    // ブロック情報テーブルのヒープスロット
    uint blockInfoTableHeapSlot;
	// 隣接チャンク１ブロック分も含めたブロックID配列のヒープスロット
    uint blockIdHeapSlot;
};

cbuffer ChunkInfo : register(b1)
{
    // 各軸のブロック数
    uint3 chunkBlockCountXYZ;
};

// グループ毎の露出面数リスト
RWStructuredBuffer<uint> outFaceCountPerGroup : register(u0);
// 全グループ共有のカウンタ(1要素)
RWStructuredBuffer<uint> faceWriteCounter : register(u1);
// グループごとの開始位置(8192要素)
RWStructuredBuffer<uint> groupOffset : register(u2);

groupshared uint gFaceCountPerThread[8];

// 3次元配列のインデックスを1次元配列のインデックスに変換する関数
uint FlattenHalo(int3 localPos)
{
    uint3 haloDim = chunkBlockCountXYZ + uint3(2, 2, 2);
    int3 h = localPos + int3(1, 1, 1);
    return (uint) h.x + (uint) h.y * haloDim.x + (uint) h.z * haloDim.x * haloDim.y;
}

[numthreads(8, 1, 1)]
void main(uint threadID : SV_GroupIndex, uint3 groupID : SV_GroupID)
{
    // ブロックID配列
    StructuredBuffer<uint> blockIds = ResourceDescriptorHeap[blockIdHeapSlot];
    
    // ブロック情報テーブル
    // x: 16進数color                  BlockInfo::color
    // y: 透過ブロックかどうか         BlockInfo::isTransparent
    // z, w: きっといつか使う
    StructuredBuffer<uint4> blockInfoTable = ResourceDescriptorHeap[blockInfoTableHeapSlot];
    
    
    // チャンクの各軸に何個の2x2x2のかたまりがあるか
    uint3 groupCountXYZ = chunkBlockCountXYZ / 2;
    // チャンクをかたまり単位で分割した時の整数座標
    uint3 groupCoord;
    groupCoord.x = groupID.x % groupCountXYZ.x;
    groupCoord.y = (groupID.x / groupCountXYZ.x) % groupCountXYZ.y;
    groupCoord.z = groupID.x / (groupCountXYZ.x * groupCountXYZ.y);
    // チャンクをブロック単位で分割した時の整数座標
    int3 groupOrigin = int3(groupCoord) * 2;
    // threadIDを分解し、2x2x2内でのXYZ座標(0か1)に変換
    int3 localOffset = int3(threadID & 1, (threadID >> 1) & 1, (threadID >> 2) & 1);
    // かたまりの基準位置 ＋ 自分のローカル位置 ＝ このスレッドが担当するブロックの整数座標
    int3 voxelPos = groupOrigin + localOffset;
    
    // ブロックIDを取得
    uint blockId = blockIds[FlattenHalo(voxelPos)];
    
    // 露出している面を求める
    uint faceMask = 0;
    // blockId = Air
    if (blockId != 0)
    {
        [unroll]
        for (uint f = 0; f < 6; f++)
        {
            // 隣接ブロックのIDを取得
            uint neighborId = blockIds[FlattenHalo(voxelPos + kFaceInfos[f].faceDir)];
            // 隣接ブロックが透過ブロックなら、この面は露出している
            if (neighborId == 0 || blockInfoTable[neighborId].y != 0)
                faceMask |= (1u << f);
        }
    }
    
    // 各スレッド(各ブロック)の露出している面の数を数える  countbits(uint value) : 1の数を数える組み込み関数
    gFaceCountPerThread[threadID] = countbits(faceMask);
    // 同じ2x2x2かたまりに属してるスレッドが全部終わるまで待つ
    GroupMemoryBarrierWithGroupSync();
    
    // グループの中で一回だけ計算すればよい。
    if (threadID == 0)
    {
        // 2x2x2かたまり内で必要な面の総数を計算する
        uint offset = 0;
        [unroll]
        for (uint i = 0; i < 8; i++)
        {
            // offset += ブロック[i]の露出面数
            offset += gFaceCountPerThread[i];
        }
        // このグループの総露出面数を記録
        outFaceCountPerGroup[groupID.x] = offset;
        
        
        // faceWriteCounter[0]にこのグループの総露出面数を加算
        // groupBaseには総露出面数が加算される前の値が入る
        uint groupBase;
        InterlockedAdd(faceWriteCounter[0], offset, groupBase);
        // このgroupIDのオフセットを記録する
        groupOffset[groupID.x] = groupBase;
    }
}
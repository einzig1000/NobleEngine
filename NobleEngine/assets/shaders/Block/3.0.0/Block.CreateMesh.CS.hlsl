#include "Block.hlsli"

cbuffer SrvIndex : register(b0)
{
    // ブロック情報テーブルのSRVスロット
    uint blockInfoTableSrvIndex;
	// 隣接チャンク１ブロック分も含めたブロックID配列SRVスロット
    uint blockIdSrvIndex;
};

cbuffer ChunkInfo : register(b1)
{
    uint3 chunkDim;
};

RWStructuredBuffer<BakedFace> outFaces : register(u0);
RWStructuredBuffer<uint> outFaceCountPerGroup : register(u1);

groupshared uint gFaceCountPerThread[8];
groupshared uint gFaceOffsetPerThread[8];


uint FlattenHalo(int3 localPos)
{
    uint3 haloDim = chunkDim + uint3(2, 2, 2);
    int3 h = localPos + int3(1, 1, 1);
    return (uint) h.x + (uint) h.y * haloDim.x + (uint) h.z * haloDim.x * haloDim.y;
}

[numthreads(8, 1, 1)]
void main(uint gtid : SV_GroupIndex, uint3 gid : SV_GroupID)
{
    // ブロックID配列
    StructuredBuffer<uint> blockIds = ResourceDescriptorHeap[blockIdSrvIndex];
    
    // ブロック情報テーブル
    // x: 16進数color                  BlockInfo::color
    // y: 透過ブロックかどうか         BlockInfo::isTransparent
    // z, w: きっといつか使う
    StructuredBuffer<uint4> blockInfoTable = ResourceDescriptorHeap[blockInfoTableSrvIndex];
    
    
    // チャンクの各軸に何個の2x2x2のかたまりがあるか
    uint3 groupDim = chunkDim / 2;
    // チャンクをかたまり単位で分割した時の整数座標
    uint3 groupCoord;
    groupCoord.x = gid.x % groupDim.x;
    groupCoord.y = (gid.x / groupDim.x) % groupDim.y;
    groupCoord.z = gid.x / (groupDim.x * groupDim.y);
    // チャンクをブロック単位で分割した時の整数座標
    int3 groupOrigin = int3(groupCoord) * 2;
    // gtidを分解し、2x2x2内でのXYZ座標(0か1)に変換
    int3 localOffset = int3(gtid & 1, (gtid >> 1) & 1, (gtid >> 2) & 1);
    // かたまりの基準位置 ＋ 自分のローカル位置 ＝ このスレッドが担当するブロックの整数座標
    int3 voxelPos = groupOrigin + localOffset;
    
    // ブロックIDを取得
    uint blockId = blockIds[FlattenHalo(voxelPos)];
    
    // 露出している面を求める
    uint faceMask = 0;
    if (blockId != 0)
    {
        [unroll]
        for (uint f = 0; f < 6; f++)
        {
            // 隣接ブロックのIDを取得
            uint neighborId = blockIds[FlattenHalo(voxelPos + kFaceInfos[f].faceDir)];
            // 隣接ブロックが透過ブロックなら、この面は露出している
            bool neighborIsAirOrTransparent = (neighborId == 0) || (blockInfoTable[neighborId].y != 0);
            if (neighborIsAirOrTransparent)
                faceMask |= (1u << f);
        }
    }
    
    // 各スレッド(各ブロック)の露出している面の数を数える  countbits(uint value) : 1の数を数える組み込み関数
    gFaceCountPerThread[gtid] = countbits(faceMask);
    // 同じ2x2x2かたまりに属してるスレッドが全部終わるまで待つ
    GroupMemoryBarrierWithGroupSync();
    
    // グループの中で一回だけ計算すればよい。
    if (gtid == 0)
    {
        // 2x2x2かたまり内で必要な面の総数を計算する
        uint offset = 0;
        [unroll]
        for (uint i = 0; i < 8; i++)
        {
            // 面配列の先頭から、ブロックごとの面の数を足し合わせてオフセットを計算する
            gFaceOffsetPerThread[i] = offset;
            // offset += ブロック[i]の露出面数
            offset += gFaceCountPerThread[i];
        }
        // このグループの総露出面数を記録
        outFaceCountPerGroup[gid.x] = offset;
    }
    
    // gtid 1～7を待たせる
    GroupMemoryBarrierWithGroupSync();

    // このスレッドが担当するブロックの面
    uint faceOut = gFaceOffsetPerThread[gtid];
    // チャンク内全ブロックを通し番号で管理する
    uint outBase = gid.x * kMaxFacesPerGroup;

    [unroll]
    for (uint f = 0; f < 6; f++)
    {
        if (faceMask & (1u << f))
        {
            BakedFace face;
            // ブロックの整数座標をuint型にパックする
            face.packedVoxelPos = (uint) voxelPos.x | ((uint) voxelPos.y << 8) | ((uint) voxelPos.z << 16);
            // どの面かを記録(kFaceInfos[6]と対応)
            face.faceIndex = f;
            // ブロックIDを記録
            face.blockId = blockId;

            // 各グループに48枠用意する。
            outFaces[outBase + faceOut] = face;
            faceOut++;
        }
    }
}
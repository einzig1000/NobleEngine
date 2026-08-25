#include "../Utilities/Utilities.hlsli"

// ブロックIDを毎フレーム受け取り、その場で頂点を計算して描画するメッシュシェーダー(AS無し版)
// 2x2x2ボクセルを1グループとして処理する。DispatchMeshは (グループ総数, 1, 1) の1次元で呼ぶ想定。

static const float3 kCubeCorners[8] =
{
    float3(0, 0, 0), float3(1, 0, 0), float3(0, 1, 0), float3(1, 1, 0),
    float3(0, 0, 1), float3(1, 0, 1), float3(0, 1, 1), float3(1, 1, 1),
};

// 各面を構成する4頂点(kCubeCornersへのインデックス)。外向き法線に対してCCW巻き順。
static const uint4 kFaceCorners[6] =
{
    uint4(1, 3, 7, 5), // +X
    uint4(0, 4, 6, 2), // -X
    uint4(2, 6, 7, 3), // +Y
    uint4(0, 1, 5, 4), // -Y
    uint4(5, 7, 6, 4), // +Z
    uint4(0, 2, 3, 1), // -Z
};

static const float3 kFaceNormal[6] =
{
    float3(1, 0, 0), float3(-1, 0, 0),
    float3(0, 1, 0), float3(0, -1, 0),
    float3(0, 0, 1), float3(0, 0, -1),
};

static const int3 kFaceDir[6] =
{
    int3(1, 0, 0), int3(-1, 0, 0),
    int3(0, 1, 0), int3(0, -1, 0),
    int3(0, 0, 1), int3(0, 0, -1),
};



struct MSOutput
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR0;
};

cbuffer ChunkInfo : register(b0)
{
	// 隣接チャンク１ブロック分も含めたブロックID配列SRVスロット
    uint blockIdSrvIndex;
    // チャンクのボクセル数 (Constexprs::kChunkX/Y/Z、各軸偶数である必要あり)
    uint3 chunkDim;
    // このチャンクのワールド座標原点
    float3 chunkWorldOrigin;
    // ブロックサイズ (Constexprs::kBlockSize)
    float blockSize;
};

cbuffer BlockInfoTableSrvIndex : register(b1)
{
    // ブロック情報テーブルのSRVスロット
    uint blockInfoTableSrvIndex;
};

cbuffer WVP : register(b2)
{
    float4x4 wvp;
}

// 2x2x2グループ内でのスレッド数と、そこから出る最大面数(8ボクセル×6面=48面)ぶんの領域
groupshared uint gFaceCountPerThread[8];
groupshared uint gFaceOffsetPerThread[8];

uint FlattenHalo(int3 localPos)
{
    // ハロー込みバッファは各軸 chunkDim+2 のサイズ。localPosは-1〜chunkDim(境界含む)まで許容。
    uint3 haloDim = chunkDim + uint3(2, 2, 2);
    int3 h = localPos + int3(1, 1, 1);
    return (uint) h.x + (uint) h.y * haloDim.x + (uint) h.z * haloDim.x * haloDim.y;
}

[outputtopology("triangle")]
[numthreads(8, 1, 1)]
void main(
    uint gtid : SV_GroupIndex,
    uint3 gid : SV_GroupID,
    out indices uint3 tris[96],
    out vertices MSOutput verts[192]
)
{
    // ブロックID配列
    StructuredBuffer<uint> blockIds = ResourceDescriptorHeap[blockIdSrvIndex];
    // ブロック情報テーブル
    // x: 16進数color                  BlockInfo::color
    // y: 透過ブロックかどうか         BlockInfo::isTransparent
    // z, w: きっといつか使う
    StructuredBuffer<uint4> blockInfoTable = ResourceDescriptorHeap[blockInfoTableSrvIndex];
    
    // チャンクを2x2x2のグループに分割して処理する
    // この2x2x2のかたまりがメッシュレットみたいなもの。
    // 各スレッドで１かたまり担当する。
    // 8スレッドが1グループを構成する。gidはチャンク内のグループ座標。
    
    // gid.x = チャンクを2x2x2のかたまりで分割した時の合計数(チャンクのボクセル数 / 8)
    // gtid = 0〜7のスレッド番号。各スレッドが1ブロックを担当する。
    
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
            uint neighborId = blockIds[FlattenHalo(voxelPos + kFaceDir[f])];
            // 隣接ブロックが透過ブロックなら、この面は露出している
            bool neighborIsAirOrTransparent = (blockInfoTable[neighborId].y != 0);
            if (neighborIsAirOrTransparent)
                faceMask |= (1u << f);
        }
    }

    // 露出している面の数を数える  countbits(uint value) : 1の数を数える組み込み関数
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
            gFaceOffsetPerThread[i] = offset;
            offset += gFaceCountPerThread[i];
        }
    }
    
    // gtid 1～7を待たせる
    GroupMemoryBarrierWithGroupSync();
    
    // 総頂点数, 総プリミティブ数を送る
    uint totalFaceCount = gFaceOffsetPerThread[7] + gFaceCountPerThread[7];
    SetMeshOutputCounts(totalFaceCount * 4, totalFaceCount * 2);
    
    // 自スレッドに割り当てられた書き込み開始面番号を取得
    uint faceOut = gFaceOffsetPerThread[gtid];
    // このブロックのワールド空間での原点座標（左下奥）を計算
    float3 worldVoxelOrigin = chunkWorldOrigin + float3(voxelPos) * blockSize;
    // ブロックの属性テーブルから色情報を取り出して float4(RGBA) に変換
    uint4 info = blockInfoTable[blockId];
    float4 color = UnpackColor(info.x);
    color.x += rand3dTo1d(voxelPos) * 0.1;
    color.y += rand3dTo1d(voxelPos) * 0.1;
    color.z += rand3dTo1d(voxelPos) * 0.1;
    //float4 color = float4(0.f, 0.f, 0.f, 1.0f);

    // 6面ループ
    [unroll]
    for (uint f2 = 0; f2 < 6; f2++)
    {
        // f2が露出している面なら、頂点を計算して書き込む
        if (faceMask & (1u << f2))
        { 
            // 頂点バッファの書き込み開始位置 (1面 = 4頂点)
            uint vBase = faceOut * 4;
            // ポリゴンバッファの書き込み開始位置 (1面 = 2三角形)
            uint pBase = faceOut * 2;
            // この面(f2)を構成する4つの頂点インデックス(0〜7)をテーブルから取得
            uint4 corners = kFaceCorners[f2];

            // 4頂点ループ
            [unroll]
            for (uint c = 0; c < 4; c++)
            {
                // 頂点のワールド座標 = ボクセルの原点 +(角のローカル位置 x 1マスのサイズ)
                float3 worldPos = worldVoxelOrigin + kCubeCorners[corners[c]] * blockSize;

                MSOutput vOut;
                vOut.position = mul(float4(worldPos, 1.0f), wvp);
                vOut.normal = kFaceNormal[f2];
                vOut.color = color;
                verts[vBase + c] = vOut;
            }

            tris[pBase + 0] = uint3(vBase, vBase + 2, vBase + 1);
            tris[pBase + 1] = uint3(vBase, vBase + 3, vBase + 2);

            faceOut++;
        }
    }
}

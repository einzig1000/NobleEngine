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

cbuffer WVP : register(b1)
{
    float4x4 wvp;
}

StructuredBuffer<uint> blockIds : register(t0);

// x: 16進数color                  BlockInfo::color
// y: 透過ブロックかどうか         BlockInfo::isTransparent
// z, w: きっといつか使う
StructuredBuffer<uint4> BlockInfoTable : register(t1);

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

float4 UnpackColor(uint c)
{
    float r = ((c >> 24) & 0xFF) / 255.0f;
    float g = ((c >> 16) & 0xFF) / 255.0f;
    float b = ((c >> 8) & 0xFF) / 255.0f;
    float a = (c & 0xFF) / 255.0f;
    return float4(r, g, b, a);
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
    // 総頂点数, 総プリミティブ数を送る
    SetMeshOutputCounts(192, 96);
    
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
    
    // 自スレッドに割り当てられた書き込み開始面番号を取得
    uint faceOut = gtid * 6;
    // このブロックのワールド空間での原点座標（左下奥）を計算
    float3 worldVoxelOrigin = chunkWorldOrigin + float3(voxelPos) * blockSize;
    // ブロックの属性テーブルから色情報を取り出して float4(RGBA) に変換
    uint blockId = blockIds[FlattenHalo(voxelPos)];
    uint4 info = BlockInfoTable[blockId];
    float4 color = UnpackColor(info.x);

    // 6面ループ
    [unroll]
    for (uint f2 = 0; f2 < 6; f2++)
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

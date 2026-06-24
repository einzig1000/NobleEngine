// ピクセルシェーダーに渡す構造体
struct MSOutput
{
    float4 position : SV_Position;
    nointerpolation uint instancedID : TEXCOORD0;
    nointerpolation uint baseTextureID : TEXCOORD1;
    nointerpolation uint breakTextureID : TEXCOORD2;
    float2 texcoord : TEXCOORD3;
    float2 texcoord2 : TEXCOORD4;
    float3 normal : TEXCOORD5;
    float4 color : COLOR0;
};

struct InstanceData
{
    float4x4 World;
    float4 Color;
    uint BaseTextureID;
    uint BreakTextureID;
};

//struct InstanceData
//{
//    float4x4 World;
//    float4 Color;
//    uint BaseTextureTopID;
//    uint BaseTextureSide1ID;
//    uint BaseTextureSide2ID;
//    uint BaseTextureSide3ID;
//    uint BaseTextureSide4ID;
//    uint BaseTextureBottomID;
//    uint BreakTextureID;
//};

StructuredBuffer<InstanceData> instanceData : register(t0);

cbuffer ViewProjection : register(b0)
{
    float4x4 viewProjection;
}

static const float3 basePositions[8] =
{
    float3(-0.05, 0.05, -0.05),
    float3(0.05, 0.05, -0.05),
    float3(0.05, -0.05, -0.05), 
    float3(-0.05, -0.05, -0.05),
    float3(-0.05, 0.05, 0.05),
    float3(0.05, 0.05, 0.05),
    float3(0.05, -0.05, 0.05),
    float3(-0.05, -0.05, 0.05)
};

static const uint faceIndices[24] =
{
    0, 1, 2, 3, // 0: Front
    5, 4, 7, 6, // 1: Back
    4, 0, 3, 7, // 2: Left
    1, 5, 6, 2, // 3: Right
    4, 5, 1, 0, // 4: Top
    3, 2, 6, 7 // 5: Bottom
};

// 6面の法線
static const float3 faceNormals[6] =
{
    float3(0, 0, -1), float3(0, 0, 1), float3(-1, 0, 0),
    float3(1, 0, 0), float3(0, 1, 0), float3(0, -1, 0)
};

// 面ごとのUV座標(4頂点分)
static const float2 baseUVs[4] =
{
    float2(0.0, 0.0), float2(1.0, 0.0), float2(1.0, 1.0), float2(0.0, 1.0)
};

// 1つのグループに3スレッド（三角形の頂点数分）を割り当てる
// 3次元配列のノリ
// [numthreads(スレッドグループ数, x軸, y軸)]最終的なスレッド数はグループ数 * x軸 * y軸
[NumThreads(24, 1, 1)]
[OutputTopology("triangle")]
void main(
    uint gtid : SV_GroupThreadID,       // グループ内スレッドID (0〜23: 各頂点を担当)
    uint gid : SV_GroupID,              // グループID (これがブロックのインスタンスIDになる)
    out vertices MSOutput verts[24],    // 出力する頂点配列 (最大24)
    out indices uint3 tris[12]          // 出力するインデックス配列 (最大12)
)
{
    // このスレッドグループが出力する【頂点数】と【プリミティブ（三角形）数】を確定させる
    // Cube → 頂点数は24、プリミティブ数は12
    SetMeshOutputCounts(24, 12);
    
    InstanceData inst = instanceData[gid];

    if (gtid < 12)
    {
        uint faceID = gtid / 2;     // どの面か (0〜5)
        uint triID = gtid % 2;      // 面内のどの三角形か (0 または 1)
        uint vBase = faceID * 4;    // その面の開始頂点番号 (0, 4, 8, 12, 16, 20)

        if (triID == 0)
            tris[gtid] = uint3(vBase, vBase + 1, vBase + 2);
        else
            tris[gtid] = uint3(vBase, vBase + 2, vBase + 3);
    }

    MSOutput output;
    
    uint faceIdx = gtid / 4;    // どの面か (0〜5)
    uint vertIdx = gtid % 4;    // 面内のどの頂点か (0〜3)

    // --- ローカル座標からワールド、スクリーン座標への変換 ---
    float4 localPos = float4(basePositions[faceIndices[gtid]], 1.0f);
    float4 worldPos = mul(localPos, inst.World);
    output.position = mul(worldPos, viewProjection);

    // --- 法線、UV、色の適用 ---
    output.normal = normalize(mul(faceNormals[faceIdx], (float3x3) inst.World));
    output.texcoord = baseUVs[vertIdx];
    output.texcoord2 = baseUVs[vertIdx];

    // --- インスタンスデータの引き継ぎ ---
    output.instancedID = gid;
    output.baseTextureID = inst.BaseTextureID;
    output.breakTextureID = inst.BreakTextureID;
    output.color = inst.Color;

    // 計算した頂点データを出力配列に書き込む
    verts[gtid] = output;
}
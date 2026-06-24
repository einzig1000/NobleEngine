// ピクセルシェーダーに渡す構造体
struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};



// 1つのグループに3スレッド（三角形の頂点数分）を割り当てる
// 3次元配列のノリ
// [numthreads(スレッドグループ数, x軸, y軸)]最終的なスレッド数はグループ数 * x軸 * y軸
[NumThreads(3, 1, 1)]
// 出力するプリミティブの形状を指定
[OutputTopology("triangle")]
void main(
    uint gtid : SV_GroupThreadID, // グループ内でのスレッドID (0〜2)
    out vertices VSOutput verts[3], // 出力する頂点配列（最大3）
    out indices uint3 tris[1] // 出力するインデックス配列（最大1）
)
{
    // このスレッドグループが出力する【頂点数】と【プリミティブ（三角形）数】を確定させる
    // 今回は 頂点3個、三角形1個
    SetMeshOutputCounts(3, 1);

    // インデックス（結び順）の設定は、最初のスレッド（0番）だけで行う
    if (gtid == 0)
    {
        tris[0] = uint3(0, 1, 2);
    }

    // スレッドID(0, 1, 2) に応じて、シェーダー内で座標と色を直接決める（頂点バッファ代わり）
    float4 positions[3] =
    {
        float4(0.0f, 0.5f, 0.0f, 1.0f), // 上
        float4(0.5f, -0.5f, 0.0f, 1.0f), // 右下
        float4(-0.5f, -0.5f, 0.0f, 1.0f) // 左下
    };
    float4 colors[3] =
    {
        float4(1.0f, 0.0f, 0.0f, 1.0f), // 赤
        float4(0.0f, 1.0f, 0.0f, 1.0f), // 緑
        float4(0.0f, 0.0f, 1.0f, 1.0f) // 青
    };

    // 自分のスレッド番号の頂点データを書き込む
    verts[gtid].position = positions[gtid];
    verts[gtid].texCoord = float2(0.0f, 0.0f);
    verts[gtid].normal = float3(0.0f, 0.0f, 1.0f);
}
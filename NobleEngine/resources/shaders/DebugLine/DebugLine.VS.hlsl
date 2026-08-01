// DebugLine.VS.hlsl
// SV_VertexIDでStructuredBuffer上の座標を直接読み、ラインリストとして描画するための頂点シェーダー
// 頂点バッファの中身は使わず、頂点数(=IASetPrimitiveTopologyがLINELISTの時は2点で1本のライン)だけを利用する

struct VSOutput
{
    float4 position : SV_POSITION;
};

cbuffer ViewProjection : register(b0)
{
    float4x4 viewProjection;
}

// ライン端点のワールド座標を並べたバッファ。2点ごとに1本のラインになる
StructuredBuffer<float3> gLinePoints : register(t0);

VSOutput main(uint vertexID : SV_VertexID)
{
    VSOutput output;
    float3 pos = gLinePoints[vertexID];
    output.position = mul(float4(pos, 1.0f), viewProjection);
    return output;
}

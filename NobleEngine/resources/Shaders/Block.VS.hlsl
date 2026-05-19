// Block.VS.hlsl

#include "Block.hlsli"

// 各インスタンスのワールド座標
StructuredBuffer<WorldMatrix> gWorldMatrix : register(t0);
// インスタンスごとの破壊アトラスのタイル番号
StructuredBuffer<uint> gBreakTile : register(t1);
// インスタンスごとのベースアトラスのタイル番号
StructuredBuffer<uint> gBaseTile : register(t2);
// 共通のビュー射影行列
ConstantBuffer<ViewProjectionMatrix> gViewProjection : register(b0);
// アトラス情報
ConstantBuffer<AtlasInfo> gBreakAtlasInfo : register(b1);
ConstantBuffer<AtlasInfo> gBaseAtlasInfo : register(b2);

// AtlasInfoから欲しいタイルのUVを計算する
static float2 ComputeAtlasUV(float2 uvInInner01, uint tileIndex, AtlasInfo info)
{
    uint tilesPerRow = max(1u, info.atlasCols);

    uint tileX = tileIndex % tilesPerRow;
    uint tileY = tileIndex / tilesPerRow;

    // 1タイル(=1面)のUV上でのサイズ（24x24想定）
    float2 strideUV = float2((float)info.faceStrideX, (float)info.faceStrideY) * info.invAtlasSize;

    // タイル左上のUV
    float2 tileOriginUV = float2((float)tileX, (float)tileY) * strideUV;

    // タイル内の「実描画領域(16x16)」の開始位置（padding=4 なら (4,4) から）
    float2 innerOffsetUV = float2((float)info.padX, (float)info.padY) * info.invAtlasSize;

    // タイル内の「実描画領域(16x16)」の大きさ
    float2 innerSizeUV = float2((float)info.innerSizeX, (float)info.innerSizeY) * info.invAtlasSize;

    // 入力(0..1)を inner 領域へマップして、atlas全体のUVに変換
    return tileOriginUV + innerOffsetUV + uvInInner01 * innerSizeUV;
}

VertexShaderOutput main(VertexShaderInput input, uint instancedID : SV_InstanceID)
{
    VertexShaderOutput output;

	// 座標変換
    float4 worldPos = mul(input.position, gWorldMatrix[instancedID].World);
    output.position = mul(worldPos, gViewProjection.ViewProjection);
    // 欲しいタイル番号取得
	uint baseTile = gBaseTile[instancedID];
    uint breakTile = gBreakTile[instancedID];
	// UV計算
    output.texcoord = ComputeAtlasUV(input.texcoord, baseTile, gBaseAtlasInfo);
    output.texcoord2 = ComputeAtlasUV(input.texcoord2, breakTile, gBreakAtlasInfo);
	// 法線変換
    output.normal = normalize(mul(input.normal, (float3x3)gWorldMatrix[instancedID].World));
	// インスタンスIDを出力
	output.instancedID = instancedID;

    return output;
}
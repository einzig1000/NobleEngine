#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <EngineDefinition/EngineDefinition.h>
#include <EngineDefinition/EngineConstexprs.h>
#include <definition/constexprs.h>

/// <summary>
/// パーリンノイズ生成クラス
/// </summary>
class PerlinNoise
{
public:
    explicit PerlinNoise(uint32_t seed = 0);

    float noise(float x, float y) const;

private:
    std::vector<int32_t> pen;

    static float fade(float t);
    static float lerp(float a, float b, float t);
    static float grad(int32_t hash, float x, float y);
};

struct NoiseParameter
{
	int32_t height = Constexprs::kChunkBlockCountY * Constexprs::kChunkStackHeight;       // マップの高さ
	float scale = 32.0f;        // 地形の粗さ（大きくすると緩やか）
    int32_t octaves = 4;            // 反復回数 (大きくすると細かい起伏が増える)
    float persistence = 0.5f;   // 各オクターブの振幅減衰 (大きくすると細かい起伏が増える)
	uint32_t seed = 12345;      // 俗に言うシード値
	PerlinNoise pn;


	// ===== 鉱石パラメータ =====
	int32_t ironVeinsPerChunk = 2;			// チャンクあたりの鉱脈数
	int32_t ironVeinSizeMean = 10;			// 平均鉱脈サイズ
	int32_t ironVeinSizeRand = 6;			// ±鉱脈サイズランダム幅
	int32_t ironMinY = 2;					// 鉱脈生成最低Y座標
	int32_t ironMaxY = Constexprs::kChunkBlockCountY - 4;			// 鉱脈生成最高Y座標

	int32_t diamondVeinsPerChunk = 1;		// チャンクあたりの鉱脈数
	int32_t diamondVeinSizeMean = 4;		// 平均鉱脈サイズ
	int32_t diamondVeinSizeRand = 3;		// ±鉱脈サイズランダム幅
	int32_t diamondMinY = 1;				// 鉱脈生成最低Y座標
	int32_t diamondMaxY = Constexprs::kChunkBlockCountY / 3;		// 鉱脈生成最高Y座標

	int32_t goldVeinsPerChunk = 1;			// チャンクあたりの鉱脈数
	int32_t goldVeinSizeMean = 6;			// 平均鉱脈サイズ
	int32_t goldVeinSizeRand = 4;			// ±鉱脈サイズランダム幅
	int32_t goldMinY = 2;					// 鉱脈生成最低Y座標
	int32_t goldMaxY = Constexprs::kChunkBlockCountY / 2;			// 鉱脈生成最高Y座標

	// ===== 木パラメータ =====
	float treeChancePerColumn = 0.02f;	// 大きいほど木が増える
	int32_t treeHeightMin = 5;				// 木の高さ最小値
	int32_t treeHeightMax = 7;				// 木の高さ最大値
	int32_t leafRadiusMin = 2;				// 葉の半径最小値
	int32_t leafRadiusMax = 3;				// 葉の半径最大値
};

// フラクタル（オクターブ）合成
float fractalPerlin(const PerlinNoise& pn, float x, float y, int32_t octaves, float persistence);
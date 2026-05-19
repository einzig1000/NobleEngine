#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <numeric>
#include "definition/definition.h"

/// <summary>
/// パーリンノイズ生成クラス
/// </summary>
class PerlinNoise
{
public:
    explicit PerlinNoise(uint32_t seed = 0);

    float noise(float x, float y) const;

private:
    std::vector<int> pen;

    static float fade(float t);
    static float lerp(float a, float b, float t);
    static float grad(int hash, float x, float y);
};

struct NoiseParameter
{
	int height = CHUNK_Y;       // マップの高さ
	float scale = 32.0f;        // 地形の粗さ（大きくすると緩やか）
    int octaves = 4;            // 反復回数 (大きくすると細かい起伏が増える)
    float persistence = 0.5f;   // 各オクターブの振幅減衰 (大きくすると細かい起伏が増える)
	uint32_t seed = 12345;      // 俗に言うシード値
	PerlinNoise pn;


	// ===== 鉱石パラメータ =====
	int ironVeinsPerChunk = 2;			// チャンクあたりの鉱脈数
	int ironVeinSizeMean = 10;			// 平均鉱脈サイズ
	int ironVeinSizeRand = 6;			// ±鉱脈サイズランダム幅
	int ironMinY = 2;					// 鉱脈生成最低Y座標
	int ironMaxY = CHUNK_Y - 4;			// 鉱脈生成最高Y座標

	int diamondVeinsPerChunk = 1;		// チャンクあたりの鉱脈数
	int diamondVeinSizeMean = 4;		// 平均鉱脈サイズ
	int diamondVeinSizeRand = 3;		// ±鉱脈サイズランダム幅
	int diamondMinY = 1;				// 鉱脈生成最低Y座標
	int diamondMaxY = CHUNK_Y / 3;		// 鉱脈生成最高Y座標

	int goldVeinsPerChunk = 1;			// チャンクあたりの鉱脈数
	int goldVeinSizeMean = 6;			// 平均鉱脈サイズ
	int goldVeinSizeRand = 4;			// ±鉱脈サイズランダム幅
	int goldMinY = 2;					// 鉱脈生成最低Y座標
	int goldMaxY = CHUNK_Y / 2;			// 鉱脈生成最高Y座標

	// ===== 木パラメータ =====
	float treeChancePerColumn = 0.02f;	// 大きいほど木が増える
	int treeHeightMin = 5;				// 木の高さ最小値
	int treeHeightMax = 7;				// 木の高さ最大値
	int leafRadiusMin = 2;				// 葉の半径最小値
	int leafRadiusMax = 3;				// 葉の半径最大値
};

// フラクタル（オクターブ）合成
float fractalPerlin(const PerlinNoise& pn, float x, float y, int octaves, float persistence);
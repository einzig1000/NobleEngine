#pragma once
#include <cstdint>
#include <vector>
#include <externals/FastNoiseLite/FastNoiseLite.h>

class TerrainGenerator
{
public:
	// 地表の高さのパラメータ(長さの単位はすべてブロック)
	struct HeightParameter
	{
		float scale = 1200.0f;		// 起伏の幅(大きいほどゆるやか)
		int32_t octaves = 8;		// ノイズを重ねる回数(多いほど細かい凹凸が増える)
		float gain = 0.7f;			// 重ねるたびの振幅の倍率(大きいほど細かい凹凸が強い)
		float baseHeight = 191.5f;	// 平均の地表の高さ
		float amplitude = 110.0f;	// 起伏の大きさ(平均から上下にどれだけ振れるか)

		float hillHeight = 0.0f;	// 原点の小山の高さ
		float hillRadius = 400.0f;	// 原点の小山の半径
	};

	TerrainGenerator();

	// シード値を設定する
	void SetSeed(uint32_t seed);
	uint32_t GetSeed() const { return seed_; }

	// 指定した列の地表の高さ(ワールドのブロック座標)。y < 高さ がブロック、y >= 高さ が空気
	int32_t SurfaceHeight(int32_t worldX, int32_t worldZ) const;

	// パラメータの調整と、上から見た高さのプレビュー
	void DrawImGui();

private:
	// パラメータをノイズに反映する
	void ApplyParameter();
	// プレビューの高さを計算し直す
	void UpdatePreview();

	// シード値
	uint32_t seed_ = 0;
	// 地表の高さのパラメータ
	HeightParameter height_;
	// 地表の高さのノイズ
	FastNoiseLite heightNoise_;

	// プレビュー
	int32_t previewSize_ = 96;		// 縦横のマス数
	int32_t previewStep_ = 16;		// 1マスあたりのブロック数
	std::vector<int32_t> previewHeights_;
	int32_t previewMin_ = 0;
	int32_t previewMax_ = 0;
	bool previewDirty_ = true;
};
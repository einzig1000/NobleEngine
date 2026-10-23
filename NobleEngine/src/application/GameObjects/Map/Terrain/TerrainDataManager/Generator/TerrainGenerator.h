#pragma once
#include <cstdint>
#include <vector>
#include <definition/definition.h>
#include <externals/FastNoiseLite/FastNoiseLite.h>

/// <summary>
/// 地形生成の計算を担当するクラス
/// 結果は「シード値」と「ワールドのブロック座標」だけで決まる
/// (チャンクの生成順や、隣のチャンクがあるかどうかに左右されない)
/// </summary>
class TerrainGenerator
{
public:
	// チャンクのブロック配列([x][y][z])
	using ChunkBlocks = BlockID[Constexprs::kChunkBlockCountX][Constexprs::kChunkBlockCountY][Constexprs::kChunkBlockCountZ];

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

	// 木のパラメータ(長さの単位はすべてブロック)
	struct TreeParameter
	{
		int32_t cellSize = 40;			// 木の間隔(40x40ブロック内に1本生える)
		float generatorRate = 0.7f;		// 森の中で、1マスに木が生える確率
		float forestScale = 700.0f;		// 大きいほど広い森になる(1.0f~)
		float forestThreshold = -0.1f;	// 大きいほど森が減る(-1~1)
		float trunkHeightMin = 45.0f;	// 最小の幹の高さ
		float trunkHeightMax = 80.0f;	// 最大の幹の高さ
		float trunkRadius = 3.5f;		// 根元の幹の太さ(半径)
		int32_t branchCountMin = 3;		// 最小の枝の数
		int32_t branchCountMax = 5;		// 最大の枝の数
		float leafRadiusMin = 10.0f;	// 最小の葉の塊の半径
		float leafRadiusMax = 16.0f;	// 最大の葉の塊の半径
	};

	TerrainGenerator();
	void DrawImGui();

	// シード値を設定する
	void SetSeed(uint32_t seed);
	uint32_t GetSeed() const { return seed_; }

	// 指定した列の地表の高さ
	int32_t SurfaceHeight(int32_t worldX, int32_t worldZ) const;

	// チャンクにかかる木を書き込む
	void PlaceTrees(const Vector3int& chunkIndex, ChunkBlocks& blocks) const;


private:
	// 幹・枝(始点から終点へ太さが変わるカプセル。座標はワールドのブロック座標)
	struct Capsule
	{
		float ax, ay, az, ra;	// 始点と始点での半径
		float bx, by, bz, rb;	// 終点と終点での半径
	};
	// 木
	struct Tree
	{
		std::vector<Capsule> woods;		// 幹と枝
		std::vector<SphereXYZ> leaves;	// 葉の塊
		float minX, minY, minZ;			// 木全体を囲む箱
		float maxX, maxY, maxZ;
		uint32_t hash;					// この木の乱数のもと(葉の間引きにも使う)
	};

	// パラメータをノイズに反映する
	void ApplyParameter();

	// 格子(cellX, cellZ)に木が生えるか。生えるなら根元の位置と乱数のもとを返す
	bool FindTreeRoot(int32_t cellX, int32_t cellZ, int32_t& rootX, int32_t& rootY, int32_t& rootZ, uint32_t& hash) const;
	// 根元の位置から木の形(図形のリスト)を作る
	void MakeTree(int32_t rootX, int32_t rootY, int32_t rootZ, uint32_t hash, Tree& tree) const;
	// 根元から木の端までの、横方向の最大距離(調べる範囲を決めるのに使う)
	int32_t MaxTreeReach() const;
	// 根元から木のてっぺんまでの最大の高さ
	float MaxTreeHeight() const;
	// 葉の塊をチャンクに書く(chunkMin = チャンクの最小の角のワールドのブロック座標)
	void DrawLeaf(const SphereXYZ& leaf, uint32_t treeHash, const Vector3int& chunkMin, ChunkBlocks& blocks) const;
	// 幹・枝をチャンクに書く
	void DrawWood(const Capsule& wood, const Vector3int& chunkMin, ChunkBlocks& blocks) const;

	// シード値
	uint32_t seed_ = 0;
	// 地表の高さのパラメータ
	HeightParameter height_;
	// 地表の高さのノイズ
	FastNoiseLite heightNoise_;
	// 木のパラメータ
	TreeParameter tree_;
	// 森の濃さのノイズ
	FastNoiseLite forestNoise_;
};
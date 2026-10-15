#include "TerrainGenerator.h"
#include <externals/imgui/imgui.h>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <utility>

namespace
{
	// 幹を何本のカプセルでつなぐか
	constexpr int32_t kTrunkSegments = 3;
	// 斜面でも根元が浮かないように、幹を地面に埋める深さ
	constexpr float kTrunkBury = 6.0f;
	// 幹の1区間が横に傾く最大の割合(区間の高さに対して)
	constexpr float kTrunkLean = 0.15f;
	// 枝の長さの最大(幹の高さに対する割合)
	constexpr float kBranchLengthMax = 0.45f;
	// てっぺんの葉の塊を、枝先の塊より何倍大きくするか
	constexpr float kTopLeafScale = 1.15f;
	// 一周の角度
	constexpr float kTwoPi = 6.28318530718f;

	int32_t FloorDiv(int32_t a, int32_t b)
	{
		return (a >= 0) ? (a / b) : -((-a + b - 1) / b);
	}



	// ハッシュ関数
	uint32_t Hash3(uint32_t a, uint32_t b, uint32_t c)
	{
		uint32_t h = (a * 0x8DA6B343u) ^ (b * 0xD8163841u) ^ (c * 0xCB1AB31Fu);
		h ^= h >> 16;
		h *= 0x7FEB352Du;
		h ^= h >> 15;
		h *= 0x846CA68Bu;
		h ^= h >> 16;
		return h;
	}

	// 決定論乱数の生成
	struct HashRandom
	{
		HashRandom(uint32_t seed) : state(seed) {}

		uint32_t state;

		uint32_t Next()
		{
			state += 0x9E3779B9u;
			uint32_t z = state;
			z = (z ^ (z >> 16)) * 0x85EBCA6Bu;
			z = (z ^ (z >> 13)) * 0xC2B2AE35u;
			return z ^ (z >> 16);
		}
		// 0以上1未満
		float Next01() { return static_cast<float>(Next() >> 8) * (1.0f / 16777216.0f); }
		// a以上b未満
		float Range(float a, float b) { return a + (b - a) * Next01(); }
		// a以上b以下
		int32_t RangeInt(int32_t a, int32_t b)
		{
			if (b <= a) return a;
			return a + static_cast<int32_t>(Next() % static_cast<uint32_t>(b - a + 1));
		}
	};

}

TerrainGenerator::TerrainGenerator()
{
	ApplyParameter();
}

void TerrainGenerator::DrawImGui()
{}

// シード値を設定する
void TerrainGenerator::SetSeed(uint32_t seed)
{
	seed_ = seed;
	ApplyParameter();
}

// 指定した列の地表の高さ
int32_t TerrainGenerator::SurfaceHeight(int32_t worldX, int32_t worldZ) const
{
	// FastNoiseLite君はfloatが好き
	const float x = static_cast<float>(worldX);
	const float z = static_cast<float>(worldZ);

	// 高さ = 基準の高さ + 起伏倍率 * ノイズ(-1 ~ 1)
	float height = height_.baseHeight + height_.amplitude * heightNoise_.GetNoise(x, z);

	// 原点の小山
	if (height_.hillRadius > 0.0f)
	{
		const float distance = std::sqrt(x * x + z * z);
		const float t = std::clamp(1.0f - distance / height_.hillRadius, 0.0f, 1.0f);
		height += height_.hillHeight * t * t * (3.0f - 2.0f * t);
	}

	// 世界の高さに収める
	return std::clamp(static_cast<int32_t>(std::round(height)), 0, Constexprs::kWorldMaxY);
}

// チャンクにかかる木を書き込む
void TerrainGenerator::PlaceTrees(const Vector3int& chunkIndex, ChunkBlocks& blocks) const
{
	// このチャンクのブロックの範囲(ワールドのブロック座標。min以上、max未満)
	const Vector3int chunkMin(
		chunkIndex.x * Constexprs::kChunkBlockCountX,
		chunkIndex.y * Constexprs::kChunkBlockCountY,
		chunkIndex.z * Constexprs::kChunkBlockCountZ);
	const Vector3int chunkMax(
		chunkMin.x + Constexprs::kChunkBlockCountX,
		chunkMin.y + Constexprs::kChunkBlockCountY,
		chunkMin.z + Constexprs::kChunkBlockCountZ);

	// このチャンクに枝や葉が届く可能性のある格子を全部調べる
	const int32_t cellSize = std::max(tree_.cellSize, 8);
	const int32_t reach = MaxTreeReach();
	const int32_t cellMinX = FloorDiv(chunkMin.x - reach, cellSize);
	const int32_t cellMaxX = FloorDiv(chunkMax.x - 1 + reach, cellSize);
	const int32_t cellMinZ = FloorDiv(chunkMin.z - reach, cellSize);
	const int32_t cellMaxZ = FloorDiv(chunkMax.z - 1 + reach, cellSize);

	Tree tree;
	for (int32_t cellZ = cellMinZ; cellZ <= cellMaxZ; ++cellZ)
	{
		for (int32_t cellX = cellMinX; cellX <= cellMaxX; ++cellX)
		{
			int32_t rootX = 0, rootY = 0, rootZ = 0;
			uint32_t hash = Hash3(seed_, static_cast<uint32_t>(cellX), static_cast<uint32_t>(cellZ));

			if (!FindTreeRoot(cellX, cellZ, rootX, rootY, rootZ, hash)) continue;

			MakeTree(rootX, rootY, rootZ, hash, tree);

			// このチャンクにかからない木は飛ばす
			if (tree.maxX < static_cast<float>(chunkMin.x) || tree.minX >= static_cast<float>(chunkMax.x)) continue;
			if (tree.maxY < static_cast<float>(chunkMin.y) || tree.minY >= static_cast<float>(chunkMax.y)) continue;
			if (tree.maxZ < static_cast<float>(chunkMin.z) || tree.minZ >= static_cast<float>(chunkMax.z)) continue;

			// 葉を先に、幹と枝を後に書く(幹と枝は葉を上書きする)
			for (const SphereXYZ& leaf : tree.leaves)
			{
				DrawLeaf(leaf, tree.hash, chunkMin, blocks);
			}
			for (const Capsule& wood : tree.woods)
			{
				DrawWood(wood, chunkMin, blocks);
			}
		}
	}
}

// パラメータをノイズに反映する
void TerrainGenerator::ApplyParameter()
{
	// 木
	tree_.cellSize = 40;			// 木の間隔(cellSize x cellSizeブロック内に1本生える)
	tree_.generatorRate = 0.7f;		// 木が生える確率
	tree_.forestScale = 700.0f;		// 大きいほど広い森になる(1.0f~)
	tree_.forestThreshold = -0.1f;	// 大きいほど森が減る(-1~1)
	tree_.trunkHeightMin = 35.0f;	// 最小の幹の高さ
	tree_.trunkHeightMax = 60.0f;	// 最大の幹の高さ
	tree_.trunkRadius = 3.5f;		// 根元の幹の太さ(半径)
	tree_.branchCountMin = 3;		// 最小の枝の数
	tree_.branchCountMax = 5;		// 最大の枝の数
	tree_.leafRadiusMin = 10.0f;	// 最小の葉の塊の半径
	tree_.leafRadiusMax = 16.0f;	// 最大の葉の塊の半径

	tree_.cellSize = std::max(tree_.cellSize, 8);
	if (tree_.trunkHeightMin > tree_.trunkHeightMax)
	{
		std::swap(tree_.trunkHeightMin, tree_.trunkHeightMax);
	}
	if (tree_.branchCountMin > tree_.branchCountMax)
	{
		std::swap(tree_.branchCountMin, tree_.branchCountMax);
	}
	if (tree_.leafRadiusMin > tree_.leafRadiusMax)
	{
		std::swap(tree_.leafRadiusMin, tree_.leafRadiusMax);
	}

	// 高さ
	height_.scale = 1200.0f;		// 起伏の幅(大きいほどゆるやか)
	height_.octaves = 8;			// ノイズを重ねる回数(多いほど細かい凹凸が増える)
	height_.gain = 0.7f;			// 重ねるたびの振幅の倍率(大きいほど細かい凹凸が強い)
	height_.baseHeight = static_cast<float>(Constexprs::kWorldMaxY) * 0.7f;	// 平均の地表の高さ
	height_.amplitude = 110.0f;		// 起伏の大きさ(平均から上下にどれだけ振れるか)
	height_.hillHeight = 0.0f;		// 原点の小山の高さ
	height_.hillRadius = 400.0f;	// 原点の小山の半径

	// 高さノイズ
	heightNoise_.SetSeed(static_cast<int>(seed_));
	heightNoise_.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	heightNoise_.SetFrequency(1.0f / std::max(height_.scale, 1.0f));
	heightNoise_.SetFractalType(FastNoiseLite::FractalType_FBm);
	heightNoise_.SetFractalOctaves(height_.octaves);
	heightNoise_.SetFractalGain(height_.gain);

	// 森の濃さ
	forestNoise_.SetSeed(static_cast<int>(seed_ + 1));
	forestNoise_.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	forestNoise_.SetFrequency(1.0f / std::max(tree_.forestScale, 1.0f));
	forestNoise_.SetFractalType(FastNoiseLite::FractalType_FBm);
	forestNoise_.SetFractalOctaves(3);
}

// 格子(cellX, cellZ)に木が生えるか
bool TerrainGenerator::FindTreeRoot(int32_t cellX, int32_t cellZ, int32_t& rootX, int32_t& rootY, int32_t& rootZ, uint32_t& hash) const
{
	// 決定論的乱数生成
	HashRandom random(hash);

	// 生える確率
	if (random.Next01() >= tree_.generatorRate) return false;

	// 根元の位置
	const int32_t margin = std::clamp(static_cast<int32_t>(std::ceil(tree_.trunkRadius * 1.2f)) + 1, 0, tree_.cellSize / 2 - 1);
	const uint32_t span = static_cast<uint32_t>(tree_.cellSize - margin * 2);
	rootX = cellX * tree_.cellSize + margin + static_cast<int32_t>(random.Next() % span);
	rootZ = cellZ * tree_.cellSize + margin + static_cast<int32_t>(random.Next() % span);

	// 森の外には生えない
	const float forest = forestNoise_.GetNoise(static_cast<float>(rootX), static_cast<float>(rootZ));
	if (forest <= tree_.forestThreshold) return false;

	// 世界の上に突き抜けてしまう高さには生やさない
	rootY = SurfaceHeight(rootX, rootZ);
	if (static_cast<float>(rootY) + MaxTreeHeight() > static_cast<float>(Constexprs::kWorldMaxY)) return false;

	return true;
}

// 根元の位置から木の形を作る
void TerrainGenerator::MakeTree(int32_t rootX, int32_t rootY, int32_t rootZ, uint32_t hash, Tree& tree) const
{
	// 形を決める乱数
	HashRandom random(hash ^ 0x5BD1E995u);
	tree.woods.clear();
	tree.leaves.clear();
	tree.hash = hash;

	const float height = random.Range(tree_.trunkHeightMin, tree_.trunkHeightMax);
	const float radius = tree_.trunkRadius * random.Range(0.8f, 1.2f);

	// 幹:少しずつ傾けたカプセルをつなぐ(根元は地面に埋めて、斜面でも浮かないようにする)
	const float segmentHeight = (height + kTrunkBury) / static_cast<float>(kTrunkSegments);
	float x = static_cast<float>(rootX) + 0.5f;
	float y = static_cast<float>(rootY) - kTrunkBury;
	float z = static_cast<float>(rootZ) + 0.5f;
	for (int32_t i = 0; i < kTrunkSegments; ++i)
	{
		Capsule trunk{};
		trunk.ax = x;
		trunk.ay = y;
		trunk.az = z;
		trunk.ra = radius * (1.0f - 0.55f * static_cast<float>(i) / static_cast<float>(kTrunkSegments));
		x += random.Range(-1.0f, 1.0f) * segmentHeight * kTrunkLean;
		y += segmentHeight;
		z += random.Range(-1.0f, 1.0f) * segmentHeight * kTrunkLean;
		trunk.bx = x;
		trunk.by = y;
		trunk.bz = z;
		trunk.rb = radius * (1.0f - 0.55f * static_cast<float>(i + 1) / static_cast<float>(kTrunkSegments));
		tree.woods.push_back(trunk);
	}

	// 幹のてっぺんの葉の塊
	const float topLeaf = random.Range(tree_.leafRadiusMin, tree_.leafRadiusMax) * kTopLeafScale;
	tree.leaves.push_back({ x, y + topLeaf * 0.3f, z, topLeaf, topLeaf * 0.75f, topLeaf });

	// 枝と、枝先の葉の塊
	const int32_t branchCount = random.RangeInt(tree_.branchCountMin, tree_.branchCountMax);
	const float startAngle = random.Range(0.0f, kTwoPi);
	for (int32_t b = 0; b < branchCount; ++b)
	{
		// 幹のどの高さから出すか(幹の途中の位置と太さを求める)
		const float t = random.Range(0.45f, 0.85f) * static_cast<float>(kTrunkSegments);
		const int32_t index = std::min(static_cast<int32_t>(t), kTrunkSegments - 1);
		const float u = t - static_cast<float>(index);
		const Capsule trunk = tree.woods[index];
		const float startX = trunk.ax + (trunk.bx - trunk.ax) * u;
		const float startY = trunk.ay + (trunk.by - trunk.ay) * u;
		const float startZ = trunk.az + (trunk.bz - trunk.az) * u;
		const float trunkRadius = trunk.ra + (trunk.rb - trunk.ra) * u;

		// 向き(全方向に散らばるように少しずつずらす)と長さ
		const float angle = startAngle + (static_cast<float>(b) + random.Range(-0.3f, 0.3f)) * kTwoPi / static_cast<float>(branchCount);
		const float up = random.Range(0.5f, 1.1f);
		const float length = height * random.Range(0.25f, kBranchLengthMax);
		const float scale = length / std::sqrt(1.0f + up * up);

		Capsule branch;
		branch.ax = startX;
		branch.ay = startY;
		branch.az = startZ;
		branch.ra = trunkRadius * 0.55f;
		branch.bx = startX + std::cos(angle) * scale;
		branch.by = startY + up * scale;
		branch.bz = startZ + std::sin(angle) * scale;
		branch.rb = std::max(0.8f, trunkRadius * 0.2f);
		tree.woods.push_back(branch);

		const float leaf = random.Range(tree_.leafRadiusMin, tree_.leafRadiusMax);
		tree.leaves.push_back({ branch.bx, branch.by + leaf * 0.2f, branch.bz, leaf, leaf * 0.7f, leaf });
	}

	// 木全体を囲む箱
	tree.minX = tree.minY = tree.minZ = FLT_MAX;
	tree.maxX = tree.maxY = tree.maxZ = -FLT_MAX;
	for (const Capsule& wood : tree.woods)
	{
		const float r = std::max(wood.ra, wood.rb);
		tree.minX = std::min(tree.minX, std::min(wood.ax, wood.bx) - r);
		tree.minY = std::min(tree.minY, std::min(wood.ay, wood.by) - r);
		tree.minZ = std::min(tree.minZ, std::min(wood.az, wood.bz) - r);
		tree.maxX = std::max(tree.maxX, std::max(wood.ax, wood.bx) + r);
		tree.maxY = std::max(tree.maxY, std::max(wood.ay, wood.by) + r);
		tree.maxZ = std::max(tree.maxZ, std::max(wood.az, wood.bz) + r);
	}
	for (const SphereXYZ& leaf : tree.leaves)
	{
		tree.minX = std::min(tree.minX, leaf.center.x - leaf.radius.x);
		tree.minY = std::min(tree.minY, leaf.center.y - leaf.radius.y);
		tree.minZ = std::min(tree.minZ, leaf.center.z - leaf.radius.z);
		tree.maxX = std::max(tree.maxX, leaf.center.x + leaf.radius.x);
		tree.maxY = std::max(tree.maxY, leaf.center.y + leaf.radius.y);
		tree.maxZ = std::max(tree.maxZ, leaf.center.z + leaf.radius.z);
	}
}

// 根元から木の端までの、横方向の最大距離
int32_t TerrainGenerator::MaxTreeReach() const
{
	//  = (幹の高さ + 根元の埋め込み) * 幹の傾き
	const float lean = (tree_.trunkHeightMax + kTrunkBury) * kTrunkLean;
	// 
	const float reach = lean + tree_.trunkHeightMax * kBranchLengthMax + tree_.leafRadiusMax * kTopLeafScale + tree_.trunkRadius * 1.2f;
	// 
	return static_cast<int32_t>(std::ceil(reach)) + 2;
}

// 根元から木のてっぺんまでの最大の高さ
float TerrainGenerator::MaxTreeHeight() const
{
	const float heightMax = std::max(tree_.trunkHeightMin, tree_.trunkHeightMax);
	const float leafMax = std::max(tree_.leafRadiusMin, tree_.leafRadiusMax);
	// 枝先は幹のてっぺんより少し高くなることがあるので、余裕を持たせる
	return heightMax * 1.2f + leafMax * 1.25f;
}

// 葉の塊をチャンクに書く
void TerrainGenerator::DrawLeaf(const SphereXYZ& leaf, uint32_t treeHash, const Vector3int& chunkMin, ChunkBlocks& blocks) const
{
	// 楕円体を囲む箱と、チャンクの重なり(チャンク内の番号)
	const int32_t x0 = std::max(0, static_cast<int32_t>(std::floor(leaf.center.x - leaf.radius.x)) - chunkMin.x);
	const int32_t x1 = std::min(Constexprs::kChunkBlockCountX - 1, static_cast<int32_t>(std::floor(leaf.center.x + leaf.radius.x)) - chunkMin.x);
	const int32_t z0 = std::max(0, static_cast<int32_t>(std::floor(leaf.center.z - leaf.radius.z)) - chunkMin.z);
	const int32_t z1 = std::min(Constexprs::kChunkBlockCountZ - 1, static_cast<int32_t>(std::floor(leaf.center.z + leaf.radius.z)) - chunkMin.z);

	for (int32_t lx = x0; lx <= x1; ++lx)
	{
		const float dx = (static_cast<float>(chunkMin.x + lx) + 0.5f - leaf.center.x) / leaf.radius.x;
		for (int32_t lz = z0; lz <= z1; ++lz)
		{
			const float dz = (static_cast<float>(chunkMin.z + lz) + 0.5f - leaf.center.z) / leaf.radius.z;
			const float rest = 1.0f - dx * dx - dz * dz;
			if (rest <= 0.0f) continue;

			// この列で楕円体の中に入る高さの範囲
			const float halfHeight = leaf.radius.y * std::sqrt(rest);
			const int32_t y0 = std::max(0, static_cast<int32_t>(std::ceil(leaf.center.y - halfHeight - 0.5f)) - chunkMin.y);
			const int32_t y1 = std::min(Constexprs::kChunkBlockCountY - 1, static_cast<int32_t>(std::floor(leaf.center.y + halfHeight - 0.5f)) - chunkMin.y);
			for (int32_t ly = y0; ly <= y1; ++ly)
			{
				BlockID& block = blocks[lx][ly][lz];
				if (block != BlockID::Air) continue;

				// 外側ほど間引いてガサガサにする(端では7割が抜ける)
				const float dy = (static_cast<float>(chunkMin.y + ly) + 0.5f - leaf.center.y) / leaf.radius.y;
				const float distanceSq = dx * dx + dy * dy + dz * dz;
				if (distanceSq > 0.55f)
				{
					const uint32_t h = Hash3(treeHash ^ static_cast<uint32_t>(chunkMin.y + ly), static_cast<uint32_t>(chunkMin.x + lx), static_cast<uint32_t>(chunkMin.z + lz));
					const float dropRate = (distanceSq - 0.55f) / 0.45f * 0.7f;
					if (static_cast<float>(h & 0xFFFFu) * (1.0f / 65536.0f) < dropRate) continue;
				}
				block = BlockID::Leaf;
			}
		}
	}
}

// 幹・枝をチャンクに書く
void TerrainGenerator::DrawWood(const Capsule& wood, const Vector3int& chunkMin, ChunkBlocks& blocks) const
{
	// カプセルを囲む箱と、チャンクの重なり(チャンク内の番号)
	const float r = std::max(wood.ra, wood.rb);
	const int32_t x0 = std::max(0, static_cast<int32_t>(std::floor(std::min(wood.ax, wood.bx) - r)) - chunkMin.x);
	const int32_t x1 = std::min(Constexprs::kChunkBlockCountX - 1, static_cast<int32_t>(std::floor(std::max(wood.ax, wood.bx) + r)) - chunkMin.x);
	const int32_t y0 = std::max(0, static_cast<int32_t>(std::floor(std::min(wood.ay, wood.by) - r)) - chunkMin.y);
	const int32_t y1 = std::min(Constexprs::kChunkBlockCountY - 1, static_cast<int32_t>(std::floor(std::max(wood.ay, wood.by) + r)) - chunkMin.y);
	const int32_t z0 = std::max(0, static_cast<int32_t>(std::floor(std::min(wood.az, wood.bz) - r)) - chunkMin.z);
	const int32_t z1 = std::min(Constexprs::kChunkBlockCountZ - 1, static_cast<int32_t>(std::floor(std::max(wood.az, wood.bz) + r)) - chunkMin.z);

	// 始点→終点のベクトル
	const float abx = wood.bx - wood.ax;
	const float aby = wood.by - wood.ay;
	const float abz = wood.bz - wood.az;
	const float lengthSq = std::max(abx * abx + aby * aby + abz * abz, 0.0001f);

	for (int32_t lx = x0; lx <= x1; ++lx)
	{
		const float px = static_cast<float>(chunkMin.x + lx) + 0.5f - wood.ax;
		for (int32_t ly = y0; ly <= y1; ++ly)
		{
			const float py = static_cast<float>(chunkMin.y + ly) + 0.5f - wood.ay;
			for (int32_t lz = z0; lz <= z1; ++lz)
			{
				BlockID& block = blocks[lx][ly][lz];
				if (block != BlockID::Air && block != BlockID::Leaf) continue;

				// 線分の上で一番近い点(0 = 始点、1 = 終点)と、そこでの太さ
				const float pz = static_cast<float>(chunkMin.z + lz) + 0.5f - wood.az;
				const float t = std::clamp((px * abx + py * aby + pz * abz) / lengthSq, 0.0f, 1.0f);
				const float qx = px - abx * t;
				const float qy = py - aby * t;
				const float qz = pz - abz * t;
				const float radius = wood.ra + (wood.rb - wood.ra) * t;
				if (qx * qx + qy * qy + qz * qz <= radius * radius)
				{
					block = BlockID::Wood;
				}
			}
		}
	}
}
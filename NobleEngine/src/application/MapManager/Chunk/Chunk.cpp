#include "MapManager/Chunk/Chunk.h"
#include "MapManager/Chunk/Block/Block.h"
#include "Utilities/JsonManager.h"
#include "MapManager/Chunk/Block/BlockConfig.h"
#include "MapManager/Chunk/Block/BlockDurability.h"

namespace
{
	// chunkごとに決定論的に乱数を出す（seed + chunk座標で固定化）
	static uint32_t Hash32(uint32_t x)
	{
		x ^= x >> 16;
		x *= 0x7feb352d;
		x ^= x >> 15;
		x *= 0x846ca68b;
		x ^= x >> 16;
		return x;
	}

	static uint32_t MakeChunkSeed(uint32_t seed, const Vector2int& chunkPos)
	{
		uint32_t h = seed;
		h ^= Hash32(static_cast<uint32_t>(chunkPos.x));
		h ^= Hash32(static_cast<uint32_t>(chunkPos.y) + 0x9e3779b9u);
		return Hash32(h);
	}

	static int RandRange(std::mt19937& rng, int minV, int maxV)
	{
		std::uniform_int_distribution<int> dist(minV, maxV);
		return dist(rng);
	}

	static float Rand01(std::mt19937& rng)
	{
		std::uniform_real_distribution<float> dist(0.0f, 1.0f);
		return dist(rng);
	}
}

Chunk::Chunk()
{
	blockConfig_ = std::make_unique<BlockConfig>();

	// ブロックデータの初期化
	for (int32_t i = 0; i < int32_t(BlockID::MAX); ++i)
	{
		blockData_ = std::make_unique<RenderData_Block>();
		blockData_->texture = ;// 全ブロックのアトラス画像
		blockData_->breakTexture = ;// 破壊アトラス画像
	}
}

Chunk::~Chunk()
{
}

// チャンクデータ生成
void Chunk::CreateChunkData(const NoiseParameter& param, const Vector2int & chunkIndex)
{
	// 永久不変のチャンク座標
	this->chunkIndex_ = chunkIndex;

	// インスタンスを作成(まじ重い)
	CreateInstance();

	// 既にセーブデータが存在している場合		(Jsonに沿ってチャンクデータを生成)
	if (loadResult)CreateChunkDataFromJson();
	// 新規生成の場合							(ノイズに沿ってチャンクデータを生成)
	else CreateChunkDataNewly(param, chunkIndex);

	// 配置後のブロックの露出状態を更新
	SetExposedAllBlocks();
}

// Jsonから読み込まれたデータを元にチャンクデータを生成
void Chunk::CreateChunkDataFromJson()
{
	// blockPositions_ に基づいてブロックを生成
	for (const auto& [blockID, positions] : blockPositions_)
	{
		for (const auto& pos : positions)
		{
			SetBlock(pos, blockID);
		}
	}
}

// 新規生成されたチャンクデータを作成
void Chunk::CreateChunkDataNewly(const NoiseParameter& param, const Vector2int& chunkIndex)
{
	// 事前に定数を計算
	const float invScale = 1.0f / param.scale;
	const int maxY = param.height - 1;

	// チャンク内すべてのブロック生成
	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int z = 0; z < CHUNK_Z; ++z)
		{
			// ワールド座標でのブロックインデックス
			const int worldX = chunkIndex.x * CHUNK_X + x;
			const int worldZ = chunkIndex.y * CHUNK_Z + z;

			// ワールド座標をノイズサンプル空間へスケールダウン
			const float sampleX = static_cast<float>(worldX) * invScale;
			const float sampleZ = static_cast<float>(worldZ) * invScale;

			// フラクタルノイズ（0..1）
			const float n = fractalPerlin(param.pn, sampleX, sampleZ, param.octaves, param.persistence);

			// 高さへ変換（0..maxY）
			int height = static_cast<int>(std::floor(n * float(maxY) + 0.5f));
			if (height < 0) height = 0;
			if (height > maxY) height = maxY;

			// 素材の割り当て
			std::mt19937 rng(MakeChunkSeed(param.seed, chunkIndex));
			int dirtThickness = RandRange(rng, 2, 5);
			if (height - dirtThickness < 0) dirtThickness = height;

			// 境界を計算（yの区間でブロックIDが決まる）
			const int stoneEnd = my_max(0, height - dirtThickness); // [0, stoneEnd)
			const int dirtEnd = my_max(0, height - 1);              // [stoneEnd, dirtEnd)
			const int lawnY = height - 1;                           // y==lawnY が Lawn（height>0のとき）

			for (int y = 0; y < CHUNK_Y; ++y)
			{
				// ブロックID決定
				BlockID id;
				
				if (y == 0)							 id = BlockID::Bedrock;
				else if (y < height - dirtThickness) id = BlockID::Stone;
				else if (y < height - 1)			 id = BlockID::Dirt;
				else if (y < height)				 id = BlockID::Lawn;
				else								 id = BlockID::Air;

				SetBlock(Vector3int(x, y, z), id);
			}
		}
	}

	GenerateOres(param);
	GenerateTrees(param);
}
// 鉱石生成
void Chunk::GenerateOres(const NoiseParameter& param)
{
	// chunkごと固定の乱数（同じseed＆chunkPosなら必ず同じ鉱脈）
	std::mt19937 rng(MakeChunkSeed(param.seed, chunkIndex_));

	auto ClampY = [&](int& minY, int& maxY)
		{
			minY = my_max(0, minY);
			maxY = my_min(CHUNK_Y - 1, maxY);
			if (minY > maxY) std::swap(minY, maxY);
		};

	// 6近傍ランダムウォーク鉱脈
	auto CarveVeins = [&](BlockID oreId, int veinsPerChunk, int sizeMean, int sizeRand, int minY, int maxY)
		{
			if (veinsPerChunk <= 0) return;
			if (sizeMean <= 0) return;

			ClampY(minY, maxY);

			static const int dx[6] = { -1, 1, 0, 0, 0, 0 };
			static const int dy[6] = { 0, 0, -1, 1, 0, 0 };
			static const int dz[6] = { 0, 0, 0, 0, -1, 1 };

			for (int v = 0; v < veinsPerChunk; ++v)
			{
				int x = RandRange(rng, 0, CHUNK_X - 1);
				int y = RandRange(rng, minY, maxY);
				int z = RandRange(rng, 0, CHUNK_Z - 1);

				int veinSize = sizeMean + RandRange(rng, -sizeRand, sizeRand);
				if (veinSize < 1) veinSize = 1;

				for (int i = 0; i < veinSize; ++i)
				{
					// Stone のみ置換（Bedrock/Dirt/Lawnは壊さない）
					if (blocks_[x][y][z]->GetBlockID() == BlockID::Stone)
					{
						SetBlock(Vector3int(x, y, z), oreId);
					}

					// 次へ（ランダムウォーク）
					const int dir = RandRange(rng, 0, 5);
					x = my_min(CHUNK_X - 1, my_max(0, x + dx[dir]));
					y = my_min(CHUNK_Y - 1, my_max(0, y + dy[dir]));
					z = my_min(CHUNK_Z - 1, my_max(0, z + dz[dir]));

					// 高さ帯から外れたら戻す（分布を安定させる）
					if (y < minY) y = minY;
					if (y > maxY) y = maxY;
				}
			}
		};

	CarveVeins(BlockID::Iron,
		param.ironVeinsPerChunk,
		param.ironVeinSizeMean,
		param.ironVeinSizeRand,
		param.ironMinY,
		param.ironMaxY);

	CarveVeins(BlockID::Diamond,
		param.diamondVeinsPerChunk,
		param.diamondVeinSizeMean,
		param.diamondVeinSizeRand,
		param.diamondMinY,
		param.diamondMaxY);
}
// 木生成
void Chunk::GenerateTrees(const NoiseParameter& param)
{
	std::mt19937 rng(MakeChunkSeed(param.seed ^ 0xA53A9C1Du, chunkIndex_));

	// 指定座標のLawnの一番上のY座標を取得
	auto FindSurfaceY_Lawn = [&](int x, int z) -> int
		{
			for (int y = CHUNK_Y - 1; y >= 0; --y)
			{
				if (blocks_[x][y][z]->GetBlockID() == BlockID::Lawn) return y;
			}
			return -1;
		};

	// 指定位置に幹を立てられるか
	auto CanPlaceTrunk = [&](int x, int y0, int z, int height) -> bool
		{
			if (y0 < 0 || y0 + height >= CHUNK_Y) return false;
			for (int y = y0; y < y0 + height; ++y)
			{
				if (blocks_[x][y][z]->GetBlockID() != BlockID::Air) return false;
			}
			return true;
		};

	// 指定位置に幹を生成した場合葉がチャンク内に収まるか
	auto CanPlaceLeaves = [&](int x, int z, int trunkTopY, int leafRadius) -> bool
		{
			if (trunkTopY - leafRadius < 0) return false;
			if (trunkTopY + leafRadius >= CHUNK_Y) return false;
			if (x - leafRadius < 0) return false;
			if (x + leafRadius >= CHUNK_X) return false;
			if (z - leafRadius < 0) return false;
			if (z + leafRadius >= CHUNK_Z) return false;
			return true;
		};

	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int z = 0; z < CHUNK_Z; ++z)
		{
			if (Rand01(rng) > param.treeChancePerColumn) continue;

			const int groundY = FindSurfaceY_Lawn(x, z);
			if (groundY < 0) continue;

			const int trunkY0 = groundY + 1;
			const int trunkH = RandRange(rng, param.treeHeightMin, param.treeHeightMax);

			// 葉半径
			const int leafRadius = RandRange(rng, param.leafRadiusMin, param.leafRadiusMax);
			const int leafCenterY = trunkY0 + trunkH - 1;

			// チャンク内に収まる木だけ作る
			if (!CanPlaceTrunk(x, trunkY0, z, trunkH)) continue;
			if (!CanPlaceLeaves(x, z, leafCenterY, leafRadius)) continue;
			if (!CanPlaceLeaves(x, z, trunkY0 + trunkH - 1, param.leafRadiusMax)) continue;

			// 幹
			for (int y = trunkY0; y < trunkY0 + trunkH; ++y)
			{
				SetBlock(Vector3int(x, y, z), BlockID::Log);
			}

			// 葉（幹先端に球っぽく）
			for (int ly = leafCenterY - leafRadius; ly <= leafCenterY + leafRadius; ++ly)
			{
				if (ly < 0 || ly >= CHUNK_Y) continue;

				for (int lx = x - leafRadius; lx <= x + leafRadius; ++lx)
				{
					if (lx < 0 || lx >= CHUNK_X) continue;

					for (int lz = z - leafRadius; lz <= z + leafRadius; ++lz)
					{
						if (lz < 0 || lz >= CHUNK_Z) continue;

						const int dx0 = lx - x;
						const int dy0 = ly - leafCenterY;
						const int dz0 = lz - z;

						// 球っぽい形
						if (dx0 * dx0 + dy0 * dy0 + dz0 * dz0 > leafRadius * leafRadius + 1) continue;

						// 空気だけ葉にする（地形と幹を潰さない）
						if (blocks_[lx][ly][lz]->GetBlockID() == BlockID::Air)
						{
							SetBlock(Vector3int(lx, ly, lz), BlockID::Leaf);
						}
					}
				}
			}
		}
	}
}


// 隣接チャンクを設定
void Chunk::SetNeighborChunk(DirectionXZ direction, Chunk* neighbor)
{
	if (direction == DirectionXZ::None) return;
	neighbors_[direction] = neighbor;
	if (neighbors_[direction])
	{
		SetExposedNeighborBlocks(direction);
	}
}

// 隣接チャンクが存在するか(生成済か)
bool Chunk::IsNeighborExist(DirectionXZ direction)
{
	return neighbors_[direction] != nullptr;
}

// localIndexのブロックの露出状態を更新
void Chunk::RefreshExposeAt(const Vector3int& localIndex)
{
	// チャンク外
	if (localIndex.x < 0 || localIndex.x >= CHUNK_X ||
		localIndex.y < 0 || localIndex.y >= CHUNK_Y ||
		localIndex.z < 0 || localIndex.z >= CHUNK_Z)
	{
		return;
	}

	Block* targetBlock = blocks_[localIndex.x][localIndex.y][localIndex.z].get();
	if (!targetBlock) return;

	// Airは「露出なし」で確定
	if (targetBlock->GetBlockID() == BlockID::Air)
	{
		targetBlock->isExposed_ = false;
		targetBlock->dataSlot_ = -1;
		return;
	}

	const bool newExposed = ComputeExposed(localIndex);
	const bool oldExposed = targetBlock->isExposed_;
	// 変化なしなら何もしない
	if (newExposed == oldExposed) return;

	targetBlock->isExposed_ = newExposed;

	// (前フレーム露出なし && 今フレーム露出あり)なら描画リストに追加
	if (newExposed)
	{
		targetBlock->dataSlot_ = blockData_->AddNewBlock(targetBlock->position_, localIndex, blockIDからタイル番号に変換);
	}
	// (前フレーム露出あり && 今フレーム露出なし)なら描画リストから削除(前フレーム露出ありの時点でdataSlot_には有効なスロット番号が入っているはず)
	else
	{
		blockData_->RemoveBlock(targetBlock->dataSlot_);
		targetBlock->dataSlot_ = -1;
	}
}
// localIndexのブロックの露出状態を判定
bool Chunk::ComputeExposed(const Vector3int& localIndex)
{
	// 範囲外（この関数は「自チャンク座標」前提）{隣接チャンクのリフレッシュはSetExposedAroundBlocks参照}
	if (localIndex.x < 0 || localIndex.x >= CHUNK_X ||
		localIndex.y < 0 || localIndex.y >= CHUNK_Y ||
		localIndex.z < 0 || localIndex.z >= CHUNK_Z)
	{
		return false;
	}

	Block* self = blocks_[localIndex.x][localIndex.y][localIndex.z].get();
	if (!self) return false;
	if (self->GetBlockID() == BlockID::Air) return false;

	// 6方向のオフセット
	static const int dx[6] = { -1, 1, 0, 0, 0, 0 };
	static const int dz[6] = { 0, 0, 0, 0, -1, 1 };
	static const int dy[6] = { 0, 0, -1, 1, 0, 0 };

	for (int i = 0; i < 6; i++)
	{
		Vector3int neighborIndex(localIndex.x + dx[i], localIndex.y + dy[i], localIndex.z + dz[i]);
		Block* neighborBlock = GetBlock(neighborIndex);

		// 隣接ブロックが存在しない(チャンクがまだ生成されていない)なら不透明ブロックとして扱う
		if (!neighborBlock) continue;
		
		// 隣接ブロックがAirか透明ブロック(葉、ガラス)なら露出している
		if (neighborBlock->GetBlockID() == BlockID::Air ||
			neighborBlock->blockInfo_.isTransparent)
		{
			return true;
		}
	}

	return false;
}

// チャンク内の全てのブロックの露出状態を更新
void Chunk::SetExposedAllBlocks()
{
	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int y = 0; y < CHUNK_Y; y++)
		{
			for (int z = 0; z < CHUNK_Z; z++)
			{
				RefreshExposeAt(Vector3int(x, y, z));
			}
		}
	}
}
// localIndexの隣接６ブロックの露出状態を更新
void Chunk::SetExposedAroundBlocks(const Vector3int& localIndex)
{
	Chunk* targetChunk = nullptr;

	// 6方向オフセット
	static const int dx[7] = { 0, -1, 1, 0, 0, 0, 0 };
	static const int dz[7] = { 0, 0, 0, 0, 0, -1, 1 };
	static const int dy[7] = { 0, 0, 0, -1, 1, 0, 0 };

	// 6方向ブロックを更新
	for (int i = 0; i < 7; ++i)
	{
		Vector3int index(localIndex.x + dx[i], localIndex.y + dy[i], localIndex.z + dz[i]);

		if (index.y < 0 || index.y >= CHUNK_Y) continue;

		if (index.x < 0)
		{
			targetChunk = neighbors_[DirectionXZ::Left];
			if (!targetChunk) continue;
			index.x += CHUNK_X; // -1 -> CHUNK_X-1
		}
		else if (index.x >= CHUNK_X)
		{
			targetChunk = neighbors_[DirectionXZ::Right];
			if (!targetChunk) continue;
			index.x -= CHUNK_X; // CHUNK_X -> 0
		}
		else if (index.z < 0)
		{
			targetChunk = neighbors_[DirectionXZ::Back];
			if (!targetChunk) continue;
			index.z += CHUNK_Z; // -1 -> CHUNK_Z-1
		}
		else if (index.z >= CHUNK_Z)
		{
			targetChunk = neighbors_[DirectionXZ::Front];
			if (!targetChunk) continue;
			index.z -= CHUNK_Z; // CHUNK_Z -> 0
		}
		else targetChunk = this;

		targetChunk->RefreshExposeAt(index);
	}
}
// チャンク境界を跨いだ近傍ブロックの露出状態を更新
void Chunk::SetExposedNeighborBlocks(const DirectionXZ direction)
{
	Chunk* neighborChunk = neighbors_[direction];
	if (!neighborChunk) return;

	switch (direction)
	{
	case DirectionXZ::Left: // -X
		for (int y = 0; y < CHUNK_Y; ++y)
		{
			for (int z = 0; z < CHUNK_Z; ++z)
			{
				neighborChunk->RefreshExposeAt(Vector3int(CHUNK_X - 1, y, z));
				neighborChunk->RefreshExposeAt(Vector3int(CHUNK_X - 2, y, z));
				this->RefreshExposeAt(Vector3int(0, y, z));	
				this->RefreshExposeAt(Vector3int(1, y, z));
			}
		}
		break;
	case DirectionXZ::Right: // +X
		for (int y = 0; y < CHUNK_Y; ++y)
		{
			for (int z = 0; z < CHUNK_Z; ++z)
			{
				neighborChunk->RefreshExposeAt(Vector3int(0, y, z));
				neighborChunk->RefreshExposeAt(Vector3int(1, y, z));
				this->RefreshExposeAt(Vector3int(CHUNK_X - 1, y, z));
				this->RefreshExposeAt(Vector3int(CHUNK_X - 2, y, z));
			}
		}
		break;
	case DirectionXZ::Back: // -Z
		for (int y = 0; y < CHUNK_Y; ++y)
		{
			for (int x = 0; x < CHUNK_X; ++x)
			{
				neighborChunk->RefreshExposeAt(Vector3int(x, y, CHUNK_Z - 1));
				neighborChunk->RefreshExposeAt(Vector3int(x, y, CHUNK_Z - 2));
				this->RefreshExposeAt(Vector3int(x, y, 0));
				this->RefreshExposeAt(Vector3int(x, y, 1));
			}
		}
		break;
	case DirectionXZ::Front: // +Z
		for (int y = 0; y < CHUNK_Y; ++y)
		{
			for (int x = 0; x < CHUNK_X; ++x)
			{
				neighborChunk->RefreshExposeAt(Vector3int(x, y, 0));
				neighborChunk->RefreshExposeAt(Vector3int(x, y, 1));
				this->RefreshExposeAt(Vector3int(x, y, CHUNK_Z - 1));
				this->RefreshExposeAt(Vector3int(x, y, CHUNK_Z - 2));
			}
		}
		break;
	default:
		break;
	}
}

void Chunk::Update() const
{
	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int z = 0; z < CHUNK_Z; z++)
		{
			for (int y = 0; y < CHUNK_Y; y++)
			{
				if (blocks_[x][y][z] != nullptr)
				{
					blocks_[x][y][z]->Update();

					if (blocks_[x][y][z]->isExposed_ && blocks_[x][y][z]->GetBlockID() != BlockID::Air)
					{
						// 色と破壊段階を描画データに反映(ほんとは変化があった時のみ呼ぶようにしたい)
						blockData_->SetColor(blocks_[x][y][z]->dataSlot_, blocks_[x][y][z]->color_);
						blockData_->SetBreakTile(blocks_[x][y][z]->dataSlot_, blocks_[x][y][z]->durability_->GetBreakStage());
					}
				}
			}
		}
	}
}

void Chunk::Draw() const
{
	blockData_->Draw();
}

// チャンクを跨いだブロックも取得できる
Block* Chunk::GetBlock(const Vector3int& index)
{
	// 存在しないブロック
	if (index.y < 0 || index.y >= CHUNK_Y) return nullptr;

	// チャンク内
	if (0 <= index.x && index.x < CHUNK_X &&
		0 <= index.y && index.y < CHUNK_Y &&
		0 <= index.z && index.z < CHUNK_Z)
	{
		return blocks_[index.x][index.y][index.z].get();
	}

	// チャンク外かつX,Z両方方向に跨いでいる場チャンクは持っていない
	const bool xOut = (index.x < 0 || index.x >= CHUNK_X);
	const bool zOut = (index.z < 0 || index.z >= CHUNK_Z);
	if (xOut && zOut) return nullptr;

	Vector3int neighborLocal = index;

	// X方向に跨ぐ && Z方向には跨いでいない
	if (index.x < 0 && !zOut)
	{
		Chunk* nb = neighbors_[DirectionXZ::Left]; // -X
		if (!nb) return nullptr;
		neighborLocal.x = neighborLocal.x + CHUNK_X;
		return nb->blocks_[neighborLocal.x][neighborLocal.y][neighborLocal.z].get();
	}
	if (index.x >= CHUNK_X && !zOut)
	{
		Chunk* nb = neighbors_[DirectionXZ::Right]; // +X
		if (!nb) return nullptr;
		neighborLocal.x = neighborLocal.x - CHUNK_X;
		return nb->blocks_[neighborLocal.x][neighborLocal.y][neighborLocal.z].get();
	}

	// Z方向に跨ぐ && X方向には跨いでいない
	if (index.z < 0 && !xOut)
	{
		Chunk* nb = neighbors_[DirectionXZ::Back]; // -Z
		if (!nb) return nullptr;
		neighborLocal.z = neighborLocal.z + CHUNK_Z;
		return nb->blocks_[neighborLocal.x][neighborLocal.y][neighborLocal.z].get();
	}
	if (index.z >= CHUNK_Z && !xOut)
	{
		Chunk* nb = neighbors_[DirectionXZ::Front]; // +Z
		if (!nb) return nullptr;
		neighborLocal.z = neighborLocal.z - CHUNK_Z;
		return nb->blocks_[neighborLocal.x][neighborLocal.y][neighborLocal.z].get();
	}

	return nullptr;
}

AABB Chunk::GetAABB(const Vector3int& index)
{
	// チャンクのワールド原点
	float chunkWorldX = chunkIndex_.x * CHUNK_X * BLOCK_SIZE;
	float chunkWorldZ = chunkIndex_.y * CHUNK_Z * BLOCK_SIZE;

	// ブロックのワールド座標
	float worldX = chunkWorldX + index.x * BLOCK_SIZE;
	float worldY = index.y * BLOCK_SIZE;
	float worldZ = chunkWorldZ + index.z * BLOCK_SIZE;

	Vector3 mint(worldX, worldY, worldZ);
	Vector3 maxt(worldX + BLOCK_SIZE, worldY + BLOCK_SIZE, worldZ + BLOCK_SIZE);

	return AABB(mint, maxt);
}

Vector3 Chunk::LocalCenter(const Vector3int& index) const
{
	const float half = BLOCK_SIZE * 0.5f;
	const float baseX = chunkIndex_.x * CHUNK_X * BLOCK_SIZE + half;
	const float baseZ = chunkIndex_.y * CHUNK_Z * BLOCK_SIZE + half;

	const float cx = baseX + index.x * BLOCK_SIZE;
	const float cy = index.y * BLOCK_SIZE + half;
	const float cz = baseZ + index.z * BLOCK_SIZE;

	return Vector3(cx, cy, cz);
}

void Chunk::SetBlock(const Vector3int& localIndex, const BlockID id) const
{
	blocks_[localIndex.x][localIndex.y][localIndex.z]->SetBlockType(blockConfig_->GetBlockInfo(id));
	blocks_[localIndex.x][localIndex.y][localIndex.z]->SetBlockPosition(LocalCenter(localIndex));
}

void Chunk::DestroyBlock(const Vector3int& localIndex)
{
	Block* block = blocks_[localIndex.x][localIndex.y][localIndex.z].get();
	if (!block) return;

	// ブロックをAirに置換
	SetBlock(localIndex, BlockID::Air);

	// ブロック側の状態フラグ
	block->isExposed_ = false;

	// 描画データから削除
	blockData_->RemoveBlock(block->dataSlot_);
	block->dataSlot_ = -1;
}

void Chunk::RebuildBlockPositions()
{}

// ブロックのインスタンス生成
void Chunk::CreateInstance()
{
	for (int x = 0; x < CHUNK_X; x++)
	{
		for (int z = 0; z < CHUNK_Z; z++)
		{
			for (int y = 0; y < CHUNK_Y; y++)
			{
				blocks_[x][y][z] = std::make_unique<Block>();
				blocks_[x][y][z]->Initialize();
			}
		}
	}
}

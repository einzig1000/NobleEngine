#include <MapManager/Chunk/Chunk.h>
#include <MapManager/Chunk/Block/Block.h>
#include <MapManager/Chunk/Block/BlockConfig.h>
#include <MapManager/Chunk/Block/BlockDurability.h>
#include <Utilities/Json/JsonManager.h>
#include <Utilities/Logger/Logger.h>
#include <ResourceLoader/ResourceID.h>

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

	static uint32_t MakeChunkSeed(uint32_t seed, const Vector3int& chunkPos)
	{
		uint32_t h = seed;
		h ^= Hash32(static_cast<uint32_t>(chunkPos.x) * 73856093u);
		h ^= Hash32(static_cast<uint32_t>(chunkPos.y) * 19349663u);
		h ^= Hash32(static_cast<uint32_t>(chunkPos.z) * 83492791u);
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
	renderData_ = std::make_unique<RenderObject>();
	renderData_->psoConfig_.vs = "resources/shaders/Block.VS.hlsl";
	renderData_->psoConfig_.ps = "resources/shaders/Block.PS.hlsl";
	renderData_->modelID_ = Game::Resource::Model::Load("resources/prototypes/model/cube/cube.obj");
	renderData_->SetupFromShaders();

	size_t blockCount = Constexprs::kChunkX * Constexprs::kChunkZ * Constexprs::kChunkY;
	instanceDataList_.reserve(blockCount);
	instanceBlockMap_.reserve(blockCount);
}

Chunk::~Chunk()
{
}

// チャンクデータ生成
void Chunk::CreateChunkData(const NoiseParameter& param, const Vector3int & chunkIndex)
{
	// 永久不変のチャンク座標
	chunkIndex_ = chunkIndex;

	// 既にセーブデータが存在している場合		(Jsonに沿ってチャンクデータを生成)
	if (loadResult)CreateChunkDataFromJson();
	// 新規生成の場合							(ノイズに沿ってチャンクデータを生成)
	else CreateChunkDataNewly(param);

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
			if (blocks_[pos.x][pos.y][pos.z].GetBlockID() != BlockID::Air)
			{
				Log("セーブデータの破損：ブロックの重複配置");
				DebugBreak();
				continue;
			}

			SetBlock(pos, blockID);
		}
	}
}

// 新規生成されたチャンクデータを作成
void Chunk::CreateChunkDataNewly(const NoiseParameter& param)
{
	// 事前に定数を計算
	const float invScale = 1.0f / param.scale;
	const int maxY = param.height - 1;

	// チャンク内すべてのブロック生成
	for (int x = 0; x < Constexprs::kChunkX; ++x)
	{
		for (int z = 0; z < Constexprs::kChunkZ; ++z)
		{
			// ワールド座標でのブロックインデックス
			const int worldX = chunkIndex_.x * Constexprs::kChunkX + x;
			const int worldZ = chunkIndex_.z * Constexprs::kChunkZ + z;

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
			std::mt19937 rng(MakeChunkSeed(param.seed, chunkIndex_));
			int dirtThickness = RandRange(rng, 2, 5);
			if (height - dirtThickness < 0) dirtThickness = height;

			// 境界を計算（yの区間でブロックIDが決まる）
			const int stoneEnd = my_max(0, height - dirtThickness); // [0, stoneEnd)
			const int dirtEnd = my_max(0, height - 1);              // [stoneEnd, dirtEnd)
			const int lawnY = height - 1;                           // y==lawnY が Lawn（height>0のとき）

			for (int y = 0; y < Constexprs::kChunkY; ++y)
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
	//GenerateTrees(param);
}
// 鉱石生成
void Chunk::GenerateOres(const NoiseParameter& param)
{
	// chunkごと固定の乱数（同じseed＆chunkPosなら必ず同じ鉱脈）
	std::mt19937 rng(MakeChunkSeed(param.seed, chunkIndex_));

	auto ClampY = [&](int& minY, int& maxY)
		{
			minY = my_max(0, minY);
			maxY = my_min(Constexprs::kChunkY - 1, maxY);
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
				int x = RandRange(rng, 0, Constexprs::kChunkX - 1);
				int y = RandRange(rng, minY, maxY);
				int z = RandRange(rng, 0, Constexprs::kChunkZ - 1);

				int veinSize = sizeMean + RandRange(rng, -sizeRand, sizeRand);
				if (veinSize < 1) veinSize = 1;

				for (int i = 0; i < veinSize; ++i)
				{
					// Stone のみ置換（Bedrock/Dirt/Lawnは壊さない）
					if (blocks_[x][y][z].GetBlockID() == BlockID::Stone)
					{
						SetBlock(Vector3int(x, y, z), oreId);
					}

					// 次へ（ランダムウォーク）
					const int dir = RandRange(rng, 0, 5);
					x = my_min(Constexprs::kChunkX - 1, my_max(0, x + dx[dir]));
					y = my_min(Constexprs::kChunkY - 1, my_max(0, y + dy[dir]));
					z = my_min(Constexprs::kChunkZ - 1, my_max(0, z + dz[dir]));

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
			for (int y = Constexprs::kChunkY - 1; y >= 0; --y)
			{
				if (blocks_[x][y][z].GetBlockID() == BlockID::Lawn) return y;
			}
			return -1;
		};

	// 指定位置に幹を立てられるか
	auto CanPlaceTrunk = [&](int x, int y0, int z, int height) -> bool
		{
			if (y0 < 0 || y0 + height >= Constexprs::kChunkY) return false;
			for (int y = y0; y < y0 + height; ++y)
			{
				if (blocks_[x][y][z].GetBlockID() != BlockID::Air) return false;
			}
			return true;
		};

	// 指定位置に幹を生成した場合葉がチャンク内に収まるか
	auto CanPlaceLeaves = [&](int x, int z, int trunkTopY, int leafRadius) -> bool
		{
			if (trunkTopY - leafRadius < 0) return false;
			if (trunkTopY + leafRadius >= Constexprs::kChunkY) return false;
			if (x - leafRadius < 0) return false;
			if (x + leafRadius >= Constexprs::kChunkX) return false;
			if (z - leafRadius < 0) return false;
			if (z + leafRadius >= Constexprs::kChunkZ) return false;
			return true;
		};

	for (int x = 0; x < Constexprs::kChunkX; ++x)
	{
		for (int z = 0; z < Constexprs::kChunkZ; ++z)
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
				if (ly < 0 || ly >= Constexprs::kChunkY) continue;

				for (int lx = x - leafRadius; lx <= x + leafRadius; ++lx)
				{
					if (lx < 0 || lx >= Constexprs::kChunkX) continue;

					for (int lz = z - leafRadius; lz <= z + leafRadius; ++lz)
					{
						if (lz < 0 || lz >= Constexprs::kChunkZ) continue;

						const int dx0 = lx - x;
						const int dy0 = ly - leafCenterY;
						const int dz0 = lz - z;

						// 球っぽい形
						if (dx0 * dx0 + dy0 * dy0 + dz0 * dz0 > leafRadius * leafRadius + 1) continue;

						// 空気だけ葉にする（地形と幹を潰さない）
						if (blocks_[lx][ly][lz].GetBlockID() == BlockID::Air)
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
void Chunk::SetNeighborChunk(DirectionXYZ direction, Chunk* neighbor)
{
	if (direction == DirectionXYZ::None) return;
	neighbors_[direction] = neighbor;
	if (neighbors_[direction])
	{
		SetExposedNeighborBlocks(direction);
	}
}

// 隣接チャンクが存在するか(生成済か)
bool Chunk::IsNeighborExist(DirectionXYZ direction)
{
	return neighbors_[direction] != nullptr;
}

// localIndexのブロックの露出状態を更新
void Chunk::RefreshExposeAt(const Vector3int& localIndex)
{
	Block* targetBlock = GetBlock(localIndex);
	if (!targetBlock) return;

	const bool preExposed = targetBlock->isExposed_;
	targetBlock->isExposed_ = ComputeExposed(localIndex);

	// 変化なしなら何もしない
	if (targetBlock->isExposed_ == preExposed) return;

	// (前フレーム露出なし && 今フレーム露出あり)なら描画リストに追加
	if (targetBlock->isExposed_)
	{
		InstanceData data;
		data.World = Matrix4x4::MakeTranslateMatrix(targetBlock->position_);
		data.BaseTextureID = ResourceID::Get3DTextureID(targetBlock->GetBlockID());
		data.BreakTextureID = 0;
		data.Color = Vector4(1, 1, 1, 1);
		int32_t slot = AllocateInstanceSlot(targetBlock, data);
	}
	// (前フレーム露出あり && 今フレーム露出なし)なら描画リストから削除(前フレーム露出ありの時点でdataSlot_には有効なスロット番号が入っているはず)
	else
	{
		FreeInstanceSlot(targetBlock);
	}

}
// localIndexのブロックの露出状態を判定
bool Chunk::ComputeExposed(const Vector3int& localIndex)
{
	Block* self = GetBlock(localIndex);
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
	for (int x = 0; x < Constexprs::kChunkX; x++)
	{
		for (int y = 0; y < Constexprs::kChunkY; y++)
		{
			for (int z = 0; z < Constexprs::kChunkZ; z++)
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

	// 6方向ブロック+自身を更新
	for (int i = 0; i < 7; ++i)
	{
		Vector3int index(localIndex.x + dx[i], localIndex.y + dy[i], localIndex.z + dz[i]);

		if (index.y < 0 || index.y >= Constexprs::kChunkY) continue;

		if (index.x < 0)
		{
			targetChunk = neighbors_[DirectionXYZ::Left];
			if (!targetChunk) continue;
			index.x += Constexprs::kChunkX; // -1 -> Constexprs::kChunkX-1
		}
		else if (index.x >= Constexprs::kChunkX)
		{
			targetChunk = neighbors_[DirectionXYZ::Right];
			if (!targetChunk) continue;
			index.x -= Constexprs::kChunkX; // Constexprs::kChunkX -> 0
		}
		else if (index.z < 0)
		{
			targetChunk = neighbors_[DirectionXYZ::Back];
			if (!targetChunk) continue;
			index.z += Constexprs::kChunkZ; // -1 -> Constexprs::kChunkZ-1
		}
		else if (index.z >= Constexprs::kChunkZ)
		{
			targetChunk = neighbors_[DirectionXYZ::Front];
			if (!targetChunk) continue;
			index.z -= Constexprs::kChunkZ; // Constexprs::kChunkZ -> 0
		}
		else if (index.y < 0)
		{
			targetChunk = neighbors_[DirectionXYZ::Down];
			if (!targetChunk) continue;
			index.y += Constexprs::kChunkY; // -1 -> Constexprs::kChunkY-1
		}
		else if (index.y >= Constexprs::kChunkY)
		{
			targetChunk = neighbors_[DirectionXYZ::Up];
			if (!targetChunk) continue;
			index.y -= Constexprs::kChunkY; // Constexprs::kChunkY -> 0
		}
		else targetChunk = this;

		targetChunk->RefreshExposeAt(index);
	}
}
// チャンク境界ブロックの露出状態を更新
void Chunk::SetExposedNeighborBlocks(const DirectionXYZ direction)
{
	Chunk* neighborChunk = neighbors_[direction];
	if (!neighborChunk) return;

	switch (direction)
	{
	case DirectionXYZ::Left: // -X
		for (int y = 0; y < Constexprs::kChunkY; ++y)
		{
			for (int z = 0; z < Constexprs::kChunkZ; ++z)
			{
				neighborChunk->RefreshExposeAt(Vector3int(Constexprs::kChunkX - 1, y, z));
				neighborChunk->RefreshExposeAt(Vector3int(Constexprs::kChunkX - 2, y, z));
				this->RefreshExposeAt(Vector3int(0, y, z));	
				this->RefreshExposeAt(Vector3int(1, y, z));
			}
		}
		break;
	case DirectionXYZ::Right: // +X
		for (int y = 0; y < Constexprs::kChunkY; ++y)
		{
			for (int z = 0; z < Constexprs::kChunkZ; ++z)
			{
				neighborChunk->RefreshExposeAt(Vector3int(0, y, z));
				neighborChunk->RefreshExposeAt(Vector3int(1, y, z));
				this->RefreshExposeAt(Vector3int(Constexprs::kChunkX - 1, y, z));
				this->RefreshExposeAt(Vector3int(Constexprs::kChunkX - 2, y, z));
			}
		}
		break;
	case DirectionXYZ::Back: // -Z
		for (int y = 0; y < Constexprs::kChunkY; ++y)
		{
			for (int x = 0; x < Constexprs::kChunkX; ++x)
			{
				neighborChunk->RefreshExposeAt(Vector3int(x, y, Constexprs::kChunkZ - 1));
				neighborChunk->RefreshExposeAt(Vector3int(x, y, Constexprs::kChunkZ - 2));
				this->RefreshExposeAt(Vector3int(x, y, 0));
				this->RefreshExposeAt(Vector3int(x, y, 1));
			}
		}
		break;
	case DirectionXYZ::Front: // +Z
		for (int y = 0; y < Constexprs::kChunkY; ++y)
		{
			for (int x = 0; x < Constexprs::kChunkX; ++x)
			{
				neighborChunk->RefreshExposeAt(Vector3int(x, y, 0));
				neighborChunk->RefreshExposeAt(Vector3int(x, y, 1));
				this->RefreshExposeAt(Vector3int(x, y, Constexprs::kChunkZ - 1));
				this->RefreshExposeAt(Vector3int(x, y, Constexprs::kChunkZ - 2));
			}
		}
		break;
	case DirectionXYZ::Down: // -Y
		for (int z = 0; z < Constexprs::kChunkZ; ++z)
		{
			for (int x = 0; x < Constexprs::kChunkX; ++x)
			{
				neighborChunk->RefreshExposeAt(Vector3int(x, Constexprs::kChunkY - 1, z));
				neighborChunk->RefreshExposeAt(Vector3int(x, Constexprs::kChunkY - 2, z));
				this->RefreshExposeAt(Vector3int(x, 0, z));
				this->RefreshExposeAt(Vector3int(x, 1, z));
			}
		}
		break;
	case DirectionXYZ::Up: // +Y
		for (int z = 0; z < Constexprs::kChunkZ; ++z)
		{
			for (int x = 0; x < Constexprs::kChunkX; ++x)
			{
				neighborChunk->RefreshExposeAt(Vector3int(x, 0, z));
				neighborChunk->RefreshExposeAt(Vector3int(x, 1, z));
				this->RefreshExposeAt(Vector3int(x, Constexprs::kChunkY - 1, z));
				this->RefreshExposeAt(Vector3int(x, Constexprs::kChunkY - 2, z));
			}
		}
		break;
	default:
		break;
	}
}

void Chunk::Update()
{
	for (int x = 0; x < Constexprs::kChunkX; x++)
	{
		for (int y = 0; y < Constexprs::kChunkY; y++)
		{
			for (int z = 0; z < Constexprs::kChunkZ; z++)
			{
				if (blocks_[x][y][z].GetBlockID() != BlockID::Air)
				{
					blocks_[x][y][z].Update();

					if (blocks_[x][y][z].isExposed_)
					{
						// 色と破壊段階を描画データに反映(ほんとは変化があった時のみ呼ぶようにしたい)
					}
				}
			}
		}
	}

	Matrix4x4 viewPro = Game::Camera::Getter::GetViewProjectionMatrix(0);
	renderData_->SetCBufferData(0, ShaderType::VertexShader, &viewPro);
	//if (instanceBufferDirty_)
	{
		renderData_->SetSBufferData(0, ShaderType::VertexShader, instanceDataList_.data(), sizeof(InstanceData), instanceDataList_.size());
		instanceBufferDirty_ = false;
	}

	renderData_->instanceNum_ = uint32_t(instanceDataList_.size());
}

void Chunk::Draw(int32_t renderTargetID)
{
	//renderData_->Draw(renderTargetID);
	renderData_->ScreenDraw();
}

// チャンクを跨いだブロックも取得できる
Block* Chunk::GetBlock(const Vector3int& index)
{
	// チャンク内
	if (0 <= index.x && index.x < Constexprs::kChunkX &&
		0 <= index.y && index.y < Constexprs::kChunkY &&
		0 <= index.z && index.z < Constexprs::kChunkZ)
	{
		return &blocks_[index.x][index.y][index.z];
	}

	Chunk* targetChunk = nullptr;
	Vector3int localIndex = index;

	if (localIndex.x < 0)
	{
		targetChunk = neighbors_[DirectionXYZ::Left];
		if (!targetChunk) return nullptr;
		localIndex.x += Constexprs::kChunkX; // -1 -> Constexprs::kChunkX-1
	}
	else if (localIndex.x >= Constexprs::kChunkX)
	{
		targetChunk = neighbors_[DirectionXYZ::Right];
		if (!targetChunk) return nullptr;
		localIndex.x -= Constexprs::kChunkX; // Constexprs::kChunkX -> 0
	}
	else if (localIndex.z < 0)
	{
		targetChunk = neighbors_[DirectionXYZ::Back];
		if (!targetChunk) return nullptr;
		localIndex.z += Constexprs::kChunkZ; // -1 -> Constexprs::kChunkZ-1
	}
	else if (localIndex.z >= Constexprs::kChunkZ)
	{
		targetChunk = neighbors_[DirectionXYZ::Front];
		if (!targetChunk) return nullptr;
		localIndex.z -= Constexprs::kChunkZ; // Constexprs::kChunkZ -> 0
	}
	else if (localIndex.y < 0)
	{
		targetChunk = neighbors_[DirectionXYZ::Down];
		if (!targetChunk) return nullptr;
		localIndex.y += Constexprs::kChunkY; // -1 -> Constexprs::kChunkY-1
	}
	else if (localIndex.y >= Constexprs::kChunkY)
	{
		targetChunk = neighbors_[DirectionXYZ::Up];
		if (!targetChunk) return nullptr;
		localIndex.y -= Constexprs::kChunkY; // Constexprs::kChunkY -> 0
	}
	else targetChunk = this;

	return targetChunk->GetBlock(localIndex);
}

AABB Chunk::GetAABB(const Vector3int& index)
{
	// チャンクのワールド原点
	float chunkWorldX = chunkIndex_.x * Constexprs::kChunkX * Constexprs::kBlockSize;
	float chunkWorldY = chunkIndex_.y * Constexprs::kChunkY * Constexprs::kBlockSize;
	float chunkWorldZ = chunkIndex_.z * Constexprs::kChunkZ * Constexprs::kBlockSize;

	// ブロックのワールド座標
	float worldX = chunkWorldX + index.x * Constexprs::kBlockSize;
	float worldY = chunkWorldY + index.y * Constexprs::kBlockSize;
	float worldZ = chunkWorldZ + index.z * Constexprs::kBlockSize;

	Vector3 mint(worldX, worldY, worldZ);
	Vector3 maxt(worldX + Constexprs::kBlockSize, worldY + Constexprs::kBlockSize, worldZ + Constexprs::kBlockSize);

	return AABB(mint, maxt);
}

Vector3 Chunk::LocalCenter(const Vector3int& index) const
{
	const float half = Constexprs::kBlockSize * 0.5f;
	const float baseX = chunkIndex_.x * Constexprs::kChunkX * Constexprs::kBlockSize + half;
	const float baseY = chunkIndex_.y * Constexprs::kChunkY * Constexprs::kBlockSize + half;
	const float baseZ = chunkIndex_.z * Constexprs::kChunkZ * Constexprs::kBlockSize + half;

	const float cx = baseX + index.x * Constexprs::kBlockSize;
	const float cy = baseY + index.y * Constexprs::kBlockSize;
	const float cz = baseZ + index.z * Constexprs::kBlockSize;

	return Vector3(cx, cy, cz);
}

void Chunk::SetBlock(const Vector3int& localIndex, const BlockID id)
{
	Block* targetBlock = GetBlock(localIndex);

	if (!targetBlock) return;

	targetBlock->SetBlockType(blockConfig_->GetBlockInfo(id));
	targetBlock->SetBlockPosition(LocalCenter(localIndex));
}

//void Chunk::DestroyBlock(const Vector3int& localIndex)
//{
//	Block* block = GetBlock(localIndex);
//	if (!block) return;
//
//	// ブロックをAirに置換
//	SetBlock(localIndex, BlockID::Air);
//
//	// ブロック側の状態フラグ
//	block->isExposed_ = false;
//
//	// 描画データから削除
//}

// 露出になったとき（割当）
int32_t Chunk::AllocateInstanceSlot(Block* b, const InstanceData& data)
{
	int32_t slot = static_cast<int32_t>(instanceDataList_.size());
	instanceDataList_.push_back(data);
	instanceBlockMap_.push_back(b);
	b->dataSlot_ = slot;
	instanceBufferDirty_ = true;
	return slot;
}

// 非露出になったとき（解放）
void Chunk::FreeInstanceSlot(Block* b)
{
	int32_t slot = b->dataSlot_;
	if (slot < 0) return;
	int32_t last = static_cast<int32_t>(instanceDataList_.size()) - 1;
	if (slot != last)
	{
		// 末尾要素を slot に移す
		instanceDataList_[slot] = instanceDataList_[last];
		Block* movedBlock = instanceBlockMap_[last];
		instanceBlockMap_[slot] = movedBlock;
		movedBlock->dataSlot_ = slot;
	}
	// 末尾を削る
	instanceDataList_.pop_back();
	instanceBlockMap_.pop_back();
	b->dataSlot_ = -1;
	instanceBufferDirty_ = true;
}

void Chunk::RebuildBlockPositions()
{}

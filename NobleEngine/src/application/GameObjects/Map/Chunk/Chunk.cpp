#include <GameObjects/Map/Chunk/Chunk.h>
#include <GameObjects/Map/Chunk/Block/Block.h>
#include <Utilities/Json/JsonManager.h>
#include <Utilities/Logger/Logger.h>
#include <Utilities/functions.h>
#include <externals/DungeonTemplateLibrary/DTL.hpp>
#include <externals/FastNoiseLite/FastNoiseLite.h>
#include <App.h>

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

	static int32_t RandRange(std::mt19937& rng, int32_t minV, int32_t maxV)
	{
		std::uniform_int_distribution<int32_t> dist(minV, maxV);
		return dist(rng);
	}

	static float Rand01(std::mt19937& rng)
	{
		std::uniform_real_distribution<float> dist(0.0f, 1.0f);
		return dist(rng);
	}
}

Chunk::Chunk(const NoiseParameter& param, const Vector3int& chunkIndex)
{
	// チャンク情報保存
	chunkIndex_ = chunkIndex;
	chunkInfo_.blockSize = Constexprs::kBlockSize;
	Vector3 chunkSize = Vector3{
		static_cast<float>(Constexprs::kChunkX) * Constexprs::kBlockSize,
		static_cast<float>(Constexprs::kChunkY) * Constexprs::kBlockSize,
		static_cast<float>(Constexprs::kChunkZ) * Constexprs::kBlockSize };
	chunkInfo_.chunkWorldOrigin = Vector3{
		static_cast<float>(chunkIndex.x) * chunkSize.x * 0.5f,
		static_cast<float>(chunkIndex.y) * chunkSize.y * 0.5f,
		static_cast<float>(chunkIndex.z) * chunkSize.z * 0.5f };
	
	// SRVスロット確保
	constexpr uint32_t groupCount = (Constexprs::kChunkX / 2) * (Constexprs::kChunkY / 2) * (Constexprs::kChunkZ / 2);
	constexpr uint32_t kMaxFacesPerGroup = 48;
	bakedFaceBufferHandle_ = Game::Resource::CreateCompute(sizeof(uint32_t) * 3, groupCount * kMaxFacesPerGroup);
	faceCountBufferHandle_ = Game::Resource::CreateCompute(sizeof(uint32_t), groupCount);
	blockIdSrvIndex_ = Game::Resource::CreateDynamic();

	// 描画オブジェクトの初期化
	render_ = std::make_unique<RenderObject>();
	render_->psoConfig_.ms = "resources/shaders/Block.Baked.MS.hlsl";
	render_->psoConfig_.ps = "resources/shaders/Block.PS.hlsl";
	render_->psoConfig_.rasterizerID = RasterizerID::Solid_FrontCull;
	render_->SetupFromShaders();
	render_->instanceNum_ = static_cast<uint32_t>((Constexprs::kChunkX * Constexprs::kChunkY * Constexprs::kChunkZ) / 8);

	// 計算オブジェクトの初期化
	compute_ = std::make_unique<ComputeObject>();
	compute_->psoConfig_.cs = "resources/shaders/Block.CreateMesh.CS.hlsl";
	compute_->SetupFromShaders();
	compute_->size = { static_cast<int32_t>(groupCount), 1, 1 };
	compute_->RegisterOutput(bakedFaceBufferHandle_);
	compute_->RegisterOutput(faceCountBufferHandle_);

	blockIds_.resize((Constexprs::kChunkX + 2) * (Constexprs::kChunkY + 2) * (Constexprs::kChunkZ + 2), 0);

	neighbors_.resize(static_cast<size_t>(DirectionXYZ::Up) + 1, nullptr);

	CreateChunkData(param);
}

Chunk::~Chunk()
{
}

#pragma region 地形生成

// チャンクデータ生成
void Chunk::CreateChunkData(const NoiseParameter& param)
{
	// 既にセーブデータが存在している場合		(Jsonに沿ってチャンクデータを生成)
	//CreateChunkDataFromJson();
	// 新規生成の場合							(ノイズに沿ってチャンクデータを生成)
	CreateChunkDataNewly(param);

	// 配置後のブロックの露出状態を更新
	//SetExposedAllBlocks();

	// 初回なので無条件でメッシュ生成
	instanceBufferDirty_ = Constexprs::kFrameCount;
}

// Jsonから読み込まれたデータを元にチャンクデータを生成
void Chunk::CreateChunkDataFromJson()
{
	/// この変数をヘッダーに
	//std::unordered_map<BlockID, std::vector<Vector3int>> blockPositions_;

	//// blockPositions_ に基づいてブロックを生成
	//for (const auto& [blockID, positions] : blockPositions_)
	//{
	//	for (const auto& pos : positions)
	//	{
	//		if (blocks_[pos.x][pos.y][pos.z].GetBlockID() != BlockID::Air)
	//		{
	//			Log("セーブデータの破損：ブロックの重複配置");
	//			DebugBreak();
	//			continue;
	//		}
	//	
	//		SetBlock(pos, blockID);
	//	}
	//}
}

/// 新規生成されたチャンクデータを作成
void Chunk::CreateChunkDataNewly(const NoiseParameter& param)
{
	// 事前に定数を計算
	const float invScale = 1.0f / param.scale;
	const int32_t maxY = param.height - 1;

	// チャンク内すべてのブロック生成
	for (int32_t x = 0; x < Constexprs::kChunkX; ++x)
	{
		for (int32_t z = 0; z < Constexprs::kChunkZ; ++z)
		{
			// ワールド座標でのブロックインデックス
			const int32_t worldX = chunkIndex_.x * Constexprs::kChunkX + x;
			const int32_t worldY = chunkIndex_.y * Constexprs::kChunkY;
			const int32_t worldZ = chunkIndex_.z * Constexprs::kChunkZ + z;

			// ワールド座標をノイズサンプル空間へスケールダウン
			const float sampleX = static_cast<float>(worldX) * invScale;
			const float sampleZ = static_cast<float>(worldZ) * invScale;

			// フラクタルノイズ（0..1）
			const float n = fractalPerlin(param.pn, sampleX, sampleZ, param.octaves, param.persistence);

			// 高さへ変換（0..maxY）
			int32_t height = static_cast<int32_t>(std::floor(n * float(maxY) + 0.5f));
			if (height < 0) height = 0;
			if (height > maxY) height = maxY;

			// 素材の割り当て
			std::mt19937 rng(MakeChunkSeed(param.seed, chunkIndex_));
			//int32_t dirtThickness = RandRange(rng, 5, 9);
			int32_t dirtThickness = int32_t(float(height) * 0.1f);
			if (height - dirtThickness < 0) dirtThickness = height;

			// 境界を計算（yの区間でブロックIDが決まる）
			const int32_t stoneEnd = std::max(0, height - dirtThickness); // [0, stoneEnd)
			const int32_t dirtEnd = std::max(0, height - 1);              // [stoneEnd, dirtEnd)
			const int32_t lawnY = height - 1;                           // y==lawnY が Lawn（height>0のとき）

			for (int32_t y = 0; y < Constexprs::kChunkY; ++y)
			{
				int32_t globalY = chunkIndex_.y * Constexprs::kChunkY + y;
				if (globalY < -1)
				{
					continue;
				}
				BlockID id;
				if (globalY == 0)							id = BlockID::Bedrock;
				else if (globalY < height - dirtThickness)	id = BlockID::Stone;
				else if (globalY < height - 1)				id = BlockID::Dirt;
				else if (globalY < height)					id = BlockID::Grass;
				else										id = BlockID::Air;
				SetBlock(Vector3int(x, y, z), id);
			}
		}
	}

	//GenerateOres(param);
	//GenerateTrees(param);
}
/// 鉱石生成
void Chunk::GenerateOres(const NoiseParameter& param)
{
	// chunkごと固定の乱数（同じseed＆chunkPosなら必ず同じ鉱脈）
	std::mt19937 rng(MakeChunkSeed(param.seed, chunkIndex_));

	auto ClampY = [&](int32_t& minY, int32_t& maxY)
		{
			minY = std::max(0, minY);
			maxY = std::min(Constexprs::kChunkY - 1, maxY);
			if (minY > maxY) std::swap(minY, maxY);
		};

	// 6近傍ランダムウォーク鉱脈
	auto CarveVeins = [&](BlockID oreId, int32_t veinsPerChunk, int32_t sizeMean, int32_t sizeRand, int32_t minY, int32_t maxY)
		{
			if (veinsPerChunk <= 0) return;
			if (sizeMean <= 0) return;

			ClampY(minY, maxY);

			static const int32_t dx[6] = { -1, 1, 0, 0, 0, 0 };
			static const int32_t dy[6] = { 0, 0, -1, 1, 0, 0 };
			static const int32_t dz[6] = { 0, 0, 0, 0, -1, 1 };

			for (int32_t v = 0; v < veinsPerChunk; ++v)
			{
				int32_t x = RandRange(rng, 0, Constexprs::kChunkX - 1);
				int32_t y = RandRange(rng, minY, maxY);
				int32_t z = RandRange(rng, 0, Constexprs::kChunkZ - 1);

				int32_t veinSize = sizeMean + RandRange(rng, -sizeRand, sizeRand);
				if (veinSize < 1) veinSize = 1;

				for (int32_t i = 0; i < veinSize; ++i)
				{
					// Stone のみ置換（Bedrock/Dirt/Lawnは壊さない）
					if (blocks_[x][y][z] == BlockID::Stone)
					{
						SetBlock(Vector3int(x, y, z), oreId);
					}

					// 次へ（ランダムウォーク）
					const int32_t dir = RandRange(rng, 0, 5);
					x = std::min(Constexprs::kChunkX - 1, std::max(0, x + dx[dir]));
					y = std::min(Constexprs::kChunkY - 1, std::max(0, y + dy[dir]));
					z = std::min(Constexprs::kChunkZ - 1, std::max(0, z + dz[dir]));

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
	auto FindSurfaceY_Lawn = [&](int32_t x, int32_t z) -> int32_t
		{
			for (int32_t y = Constexprs::kChunkY - 1; y >= 0; --y)
			{
				if (blocks_[x][y][z] == BlockID::Grass) return y;
			}
			return -1;
		};

	// 指定位置に幹を立てられるか
	auto CanPlaceTrunk = [&](int32_t x, int32_t y0, int32_t z, int32_t height) -> bool
		{
			if (y0 < 0 || y0 + height >= Constexprs::kChunkY) return false;
			for (int32_t y = y0; y < y0 + height; ++y)
			{
				if (blocks_[x][y][z] != BlockID::Air) return false;
			}
			return true;
		};

	// 指定位置に幹を生成した場合葉がチャンク内に収まるか
	auto CanPlaceLeaves = [&](int32_t x, int32_t z, int32_t trunkTopY, int32_t leafRadius) -> bool
		{
			if (trunkTopY - leafRadius < 0) return false;
			if (trunkTopY + leafRadius >= Constexprs::kChunkY) return false;
			if (x - leafRadius < 0) return false;
			if (x + leafRadius >= Constexprs::kChunkX) return false;
			if (z - leafRadius < 0) return false;
			if (z + leafRadius >= Constexprs::kChunkZ) return false;
			return true;
		};

	for (int32_t x = 0; x < Constexprs::kChunkX; ++x)
	{
		for (int32_t z = 0; z < Constexprs::kChunkZ; ++z)
		{
			if (Rand01(rng) > param.treeChancePerColumn) continue;

			const int32_t groundY = FindSurfaceY_Lawn(x, z);
			if (groundY < 0) continue;

			const int32_t trunkY0 = groundY + 1;
			const int32_t trunkH = RandRange(rng, param.treeHeightMin, param.treeHeightMax);

			// 葉半径
			const int32_t leafRadius = RandRange(rng, param.leafRadiusMin, param.leafRadiusMax);
			const int32_t leafCenterY = trunkY0 + trunkH - 1;

			// チャンク内に収まる木だけ作る
			if (!CanPlaceTrunk(x, trunkY0, z, trunkH)) continue;
			if (!CanPlaceLeaves(x, z, leafCenterY, leafRadius)) continue;
			if (!CanPlaceLeaves(x, z, trunkY0 + trunkH - 1, param.leafRadiusMax)) continue;

			// 幹
			for (int32_t y = trunkY0; y < trunkY0 + trunkH; ++y)
			{
				SetBlock(Vector3int(x, y, z), BlockID::Wood);
			}

			// 葉（幹先端に球っぽく）
			for (int32_t ly = leafCenterY - leafRadius; ly <= leafCenterY + leafRadius; ++ly)
			{
				if (ly < 0 || ly >= Constexprs::kChunkY) continue;

				for (int32_t lx = x - leafRadius; lx <= x + leafRadius; ++lx)
				{
					if (lx < 0 || lx >= Constexprs::kChunkX) continue;

					for (int32_t lz = z - leafRadius; lz <= z + leafRadius; ++lz)
					{
						if (lz < 0 || lz >= Constexprs::kChunkZ) continue;

						const int32_t dx0 = lx - x;
						const int32_t dy0 = ly - leafCenterY;
						const int32_t dz0 = lz - z;

						// 球っぽい形
						if (dx0 * dx0 + dy0 * dy0 + dz0 * dz0 > leafRadius * leafRadius + 1) continue;

						// 空気だけ葉にする（地形と幹を潰さない）
						if (blocks_[lx][ly][lz] == BlockID::Air)
						{
							SetBlock(Vector3int(lx, ly, lz), BlockID::Leaf);
						}
					}
				}
			}
		}
	}
}

//void Chunk::CreateChunkDataNewly(const NoiseParameter& param)
//{
//	FastNoiseLite noise;
//	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2); // パーリンより高速で綺麗
//	noise.SetSeed(param.seed);
//
//	// フラクタル（オクターブ）の設定
//	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
//	noise.SetFractalOctaves(4); // 4回重ねる
//
//	std::vector<std::vector<int32_t>> heightMap(Constexprs::kChunkZ, std::vector<int32_t>(Constexprs::kChunkX, 0));
//
//	for (int32_t z = 0; z < Constexprs::kChunkZ; ++z)
//	{
//		for (int32_t x = 0; x < Constexprs::kChunkX; ++x)
//		{
//			float global_x = (float)((chunkIndex_.x * Constexprs::kChunkX) + x);
//			float global_z = (float)((chunkIndex_.z * Constexprs::kChunkZ) + z);
//
//			// 2. ノイズを取得（0.05f などのスケールはここで掛けるより、関数を使うと便利）
//			// ※ FastNoiseLite は -1.0 ～ 1.0 の値を返します！
//			float raw_noise = noise.GetNoise(global_x * 0.05f, global_z * 0.05f);
//
//			// 3. -1.0～1.0 の値を 0.0～1.0 に変換する
//			float normalized_noise = (raw_noise + 1.0f) / 2.0f;
//
//			// 世界の高さはチャンクn個分
//			heightMap[z][x] = static_cast<int32_t>(normalized_noise * Constexprs::kChunkY * 30);
//		}
//	}
//
//	/// [X][Z]軸の高さに応じてブロックを配置する(chunkIndex_.Y == 0 が世界の底・chunkIndex_.Y == 30 が世界の上面)
//	for (int32_t z = 0; z < Constexprs::kChunkZ; ++z)
//	{
//		for (int32_t x = 0; x < Constexprs::kChunkX; ++x)
//		{
//			int32_t height = heightMap[z][x];
//			int32_t dirtThickness = 10;
//			for (int32_t y = 0; y < Constexprs::kChunkY; ++y)
//			{
//				int32_t globalY = chunkIndex_.y * Constexprs::kChunkY + y;
//				BlockID id;
//				if (globalY == 0)							id = BlockID::Bedrock;
//				else if (globalY < height - dirtThickness)	id = BlockID::Stone;
//				else if (globalY < height - 1)				id = BlockID::Dirt;
//				else if (globalY < height)					id = BlockID::Grass;
//				else										id = BlockID::Air;
//				SetBlock(Vector3int(x, y, z), id);
//			}
//		}
//	}
//}

#pragma endregion

#pragma region 隣接チャンク

// 隣接チャンクを設定
void Chunk::SetNeighborChunk(DirectionXYZ direction, Chunk* neighbor)
{
	if (direction == DirectionXYZ::None) return;
	neighbors_[static_cast<size_t>(direction)] = neighbor;
	instanceBufferDirty_ = Constexprs::kFrameCount;
}
// 隣接チャンクが存在するか(生成済か)
bool Chunk::IsNeighborExist(DirectionXYZ direction)
{
	return neighbors_[static_cast<size_t>(direction)] != nullptr;
}

#pragma endregion

void Chunk::Update(int32_t cameraID)
{
	// チャンクに更新が来ていたら
	if (instanceBufferDirty_ > 0)
	{
		if (instanceBufferDirty_ == Constexprs::kFrameCount)
		{
			int32_t sizeX = Constexprs::kChunkX + 2;
			int32_t sizeY = Constexprs::kChunkY + 2;
			int32_t sizeZ = Constexprs::kChunkZ + 2;

			// Z -> Y -> X の順で一次元配列に変換する
			for (int32_t z = 0; z < sizeZ; z++)
			{
				for (int32_t y = 0; y < sizeY; y++)
				{
					for (int32_t x = 0; x < sizeX; x++)
					{
						int32_t index = x + (y * sizeX) + (z * sizeX * sizeY);

						// ブロックIDを取得(隣接チャンクも探す)
						BlockID blockID = BlockID::Bedrock;
						BlockID* block = GetBlockID(Vector3int(x - 1, y - 1, z - 1), true);

						if (block != nullptr)
						{
							blockID = *block;
						}

						// ブロックIDを代入
						blockIds_[index] = static_cast<uint32_t>(blockID);
					}
				}
			}
		}

		Game::Resource::UpdateData(blockIdSrvIndex_, blockIds_.data(), sizeof(uint32_t), blockIds_.size());

		Vector2uint srvIndexTable = { Game::Resource::GetSRV(App::Data::Item::GetBlockInfoTableSRVIndex()), Game::Resource::GetSRV(blockIdSrvIndex_) };
		compute_->SetCBufferData(0, &srvIndexTable);
		Vector3int chunkDim = Vector3int(Constexprs::kChunkX, Constexprs::kChunkY, Constexprs::kChunkZ);
		compute_->SetCBufferData(1, &chunkDim);

		compute_->SetUAVData(0, Game::Resource::GetUAV(bakedFaceBufferHandle_));
		compute_->SetUAVData(1, Game::Resource::GetUAV(faceCountBufferHandle_));
		compute_->Dispatch();

		instanceBufferDirty_--;
	}

	Vector3int srvIndex = Vector3int(
		Game::Resource::GetSRV(App::Data::Item::GetBlockInfoTableSRVIndex()),
		Game::Resource::GetSRV(bakedFaceBufferHandle_),
		Game::Resource::GetSRV(faceCountBufferHandle_));
	render_->SetCBufferData(0, ShaderType::MeshShader, &srvIndex);
	render_->SetCBufferData(1, ShaderType::MeshShader, &chunkInfo_);
	Matrix4x4 viewPro = Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
	Matrix4x4 world = Matrix4x4::MakeTranslateMatrix(chunkInfo_.chunkWorldOrigin);
	Matrix4x4 wvp = world * viewPro;
	render_->SetCBufferData(2, ShaderType::MeshShader, &wvp);
}

void Chunk::Draw(int32_t renderTargetID)
{
	render_->Draw(renderTargetID);
}



BlockID* Chunk::GetBlockID(const Vector3int& index, bool checkNeighborChunk)
{
	// チャンク内
	if (0 <= index.x && index.x < Constexprs::kChunkX &&
		0 <= index.y && index.y < Constexprs::kChunkY &&
		0 <= index.z && index.z < Constexprs::kChunkZ)
	{
		return &blocks_[index.x][index.y][index.z];
	}

	if (!checkNeighborChunk) return nullptr;

	Chunk* targetChunk = nullptr;
	Vector3int localIndex = index;

	if (localIndex.x < 0)
	{
		targetChunk = neighbors_[static_cast<size_t>(DirectionXYZ::Left)];
		if (!targetChunk) return nullptr;
		localIndex.x += Constexprs::kChunkX; // -1 -> Constexprs::kChunkX-1
	}
	else if (localIndex.x >= Constexprs::kChunkX)
	{
		targetChunk = neighbors_[static_cast<size_t>(DirectionXYZ::Right)];
		if (!targetChunk) return nullptr;
		localIndex.x -= Constexprs::kChunkX; // Constexprs::kChunkX -> 0
	}
	else if (localIndex.z < 0)
	{
		targetChunk = neighbors_[static_cast<size_t>(DirectionXYZ::Back)];
		if (!targetChunk) return nullptr;
		localIndex.z += Constexprs::kChunkZ; // -1 -> Constexprs::kChunkZ-1
	}
	else if (localIndex.z >= Constexprs::kChunkZ)
	{
		targetChunk = neighbors_[static_cast<size_t>(DirectionXYZ::Front)];
		if (!targetChunk) return nullptr;
		localIndex.z -= Constexprs::kChunkZ; // Constexprs::kChunkZ -> 0
	}
	else if (localIndex.y < 0)
	{
		targetChunk = neighbors_[static_cast<size_t>(DirectionXYZ::Down)];
		if (!targetChunk) return nullptr;
		localIndex.y += Constexprs::kChunkY; // -1 -> Constexprs::kChunkY-1
	}
	else if (localIndex.y >= Constexprs::kChunkY)
	{
		targetChunk = neighbors_[static_cast<size_t>(DirectionXYZ::Up)];
		if (!targetChunk) return nullptr;
		localIndex.y -= Constexprs::kChunkY; // Constexprs::kChunkY -> 0
	}
	else targetChunk = this;

	return targetChunk->GetBlockID(localIndex);
}

AABB Chunk::GetAABB(const Vector3int& index) const
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
	BlockID* targetBlock = GetBlockID(localIndex);

	if (!targetBlock) return;

	*targetBlock = id;

	instanceBufferDirty_ = Constexprs::kFrameCount;
}

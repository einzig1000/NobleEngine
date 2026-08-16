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

	AABB CreateAABB(const AABB& aabb, const Matrix4x4& worldMatrix)
	{
		AABB result;

		// ローカルAABBの8頂点
		Vector3 corners[8] = {
			{aabb.min.x, aabb.min.y, aabb.min.z},
			{aabb.max.x, aabb.min.y, aabb.min.z},
			{aabb.min.x, aabb.max.y, aabb.min.z},
			{aabb.max.x, aabb.max.y, aabb.min.z},
			{aabb.min.x, aabb.min.y, aabb.max.z},
			{aabb.max.x, aabb.min.y, aabb.max.z},
			{aabb.min.x, aabb.max.y, aabb.max.z},
			{aabb.max.x, aabb.max.y, aabb.max.z},
		};

		// 8頂点をワールド空間に変換
		Vector3 worldMin = Transform(corners[0], worldMatrix);
		Vector3 worldMax = worldMin;
		for (int32_t i = 1; i < 8; ++i)
		{
			Vector3 v = Transform(corners[i], worldMatrix);
			worldMin.x = std::min(worldMin.x, v.x);
			worldMin.y = std::min(worldMin.y, v.y);
			worldMin.z = std::min(worldMin.z, v.z);
			worldMax.x = std::max(worldMax.x, v.x);
			worldMax.y = std::max(worldMax.y, v.y);
			worldMax.z = std::max(worldMax.z, v.z);
		}
		result = AABB(worldMin, worldMax);

		return result;
	}

	// チャンクの各軸の長さ
	constexpr Vector3 kChunkSize = Vector3{
		static_cast<float>(Constexprs::kChunkBlockCountX) * Constexprs::kBlockSize,
		static_cast<float>(Constexprs::kChunkBlockCountY) * Constexprs::kBlockSize,
		static_cast<float>(Constexprs::kChunkBlockCountZ) * Constexprs::kBlockSize };
}

Chunk::Chunk(const NoiseParameter& param, const Vector3int& chunkIndex)
{
	// チャンク情報保存
	chunkIndex_ = chunkIndex;
	chunkInfo_.chunkWorldOrigin = Vector3{
		static_cast<float>(chunkIndex.x) * kChunkSize.x,
		static_cast<float>(chunkIndex.y) * kChunkSize.y,
		static_cast<float>(chunkIndex.z) * kChunkSize.z };
	chunkAABB_ = AABB(chunkInfo_.chunkWorldOrigin, chunkInfo_.chunkWorldOrigin + kChunkSize);


	// ヒープ領域確保
	bakedFacesResourceID_ = Game::Resource::CreateCompute(sizeof(uint32_t) * 3, Constexprs::kMaxFacesPerChunk);
	//bakedFacesResourceID_ = Game::Resource::CreateCompute(sizeof(uint32_t) * 3, (kGroupCount * kMaxFacesPerGroup));
	faceCountResourceID_ = Game::Resource::CreateCompute(sizeof(uint32_t), Constexprs::kGroupCount);
	groupOffsetResourceID_ = Game::Resource::CreateCompute(sizeof(uint32_t), Constexprs::kGroupCount);
	faceWriteCounterResourceID_ = Game::Resource::CreateCompute(sizeof(uint32_t), 1);
	blockIDsResourceID_ = Game::Resource::CreateDynamic();


	// 描画オブジェクトの初期化
	render_ = std::make_unique<RenderObject>();
	//render_->psoConfig_.ms = "resources/shaders/Block/4.2.0/Block.MS.hlsl";
	render_->psoConfig_.ms = "resources/shaders/Block/4.3.0/Block.MS.hlsl";
	render_->psoConfig_.ps = "resources/shaders/Block/2.0.0/Block.PS.hlsl";
	//render_->psoConfig_.ps = "resources/shaders/Block/4.3.0/Block.PS.hlsl";
	render_->psoConfig_.as = "resources/shaders/Block/4.3.0/Block.AS.hlsl";
	render_->psoConfig_.rasterizerID = RasterizerID::Solid_FrontCull;
	render_->SetupFromShaders();
	constexpr uint32_t kASGroupSize = 32;
	render_->instanceNum_ = (Constexprs::kGroupCount + kASGroupSize - 1) / kASGroupSize;

	// 計算オブジェクトの初期化
	compute_ = std::make_unique<ComputeObject>();
	compute_->psoConfig_.cs = "resources/shaders/Block/4.3.0/Block.CreateMesh.CS.hlsl";
	compute_->SetupFromShaders();
	compute_->size = { static_cast<int32_t>(Constexprs::kGroupCount), 1, 1 };
	compute_->RegisterOutput(bakedFacesResourceID_);
	compute_->RegisterOutput(faceCountResourceID_);
	compute_->RegisterOutput(groupOffsetResourceID_);
	compute_->RegisterOutput(faceWriteCounterResourceID_);

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

	// 初回なので無条件でメッシュ生成
	blockIdsDirty_ = true;
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

	int32_t sizeX = Constexprs::kChunkBlockCountX + 2;
	int32_t sizeY = Constexprs::kChunkBlockCountY + 2;
	int32_t sizeZ = Constexprs::kChunkBlockCountZ + 2;

	// チャンク内すべてのブロック生成
	for (int32_t x = 0; x < Constexprs::kChunkBlockCountX; ++x)
	{
		for (int32_t z = 0; z < Constexprs::kChunkBlockCountZ; ++z)
		{
			// ワールド座標でのブロックインデックス
			const int32_t worldX = chunkIndex_.x * Constexprs::kChunkBlockCountX + x;
			const int32_t worldY = chunkIndex_.y * Constexprs::kChunkBlockCountY;
			const int32_t worldZ = chunkIndex_.z * Constexprs::kChunkBlockCountZ + z;

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

			for (int32_t y = 0; y < Constexprs::kChunkBlockCountY; ++y)
			{
				int32_t globalY = chunkIndex_.y * Constexprs::kChunkBlockCountY + y;
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
			maxY = std::min(Constexprs::kChunkBlockCountY - 1, maxY);
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
				int32_t x = RandRange(rng, 0, Constexprs::kChunkBlockCountX - 1);
				int32_t y = RandRange(rng, minY, maxY);
				int32_t z = RandRange(rng, 0, Constexprs::kChunkBlockCountZ - 1);

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
					x = std::min(Constexprs::kChunkBlockCountX - 1, std::max(0, x + dx[dir]));
					y = std::min(Constexprs::kChunkBlockCountY - 1, std::max(0, y + dy[dir]));
					z = std::min(Constexprs::kChunkBlockCountZ - 1, std::max(0, z + dz[dir]));

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
			for (int32_t y = Constexprs::kChunkBlockCountY - 1; y >= 0; --y)
			{
				if (blocks_[x][y][z] == BlockID::Grass) return y;
			}
			return -1;
		};

	// 指定位置に幹を立てられるか
	auto CanPlaceTrunk = [&](int32_t x, int32_t y0, int32_t z, int32_t height) -> bool
		{
			if (y0 < 0 || y0 + height >= Constexprs::kChunkBlockCountY) return false;
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
			if (trunkTopY + leafRadius >= Constexprs::kChunkBlockCountY) return false;
			if (x - leafRadius < 0) return false;
			if (x + leafRadius >= Constexprs::kChunkBlockCountX) return false;
			if (z - leafRadius < 0) return false;
			if (z + leafRadius >= Constexprs::kChunkBlockCountZ) return false;
			return true;
		};

	for (int32_t x = 0; x < Constexprs::kChunkBlockCountX; ++x)
	{
		for (int32_t z = 0; z < Constexprs::kChunkBlockCountZ; ++z)
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
				if (ly < 0 || ly >= Constexprs::kChunkBlockCountY) continue;

				for (int32_t lx = x - leafRadius; lx <= x + leafRadius; ++lx)
				{
					if (lx < 0 || lx >= Constexprs::kChunkBlockCountX) continue;

					for (int32_t lz = z - leafRadius; lz <= z + leafRadius; ++lz)
					{
						if (lz < 0 || lz >= Constexprs::kChunkBlockCountZ) continue;

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

#pragma endregion

#pragma region 隣接チャンク

// 隣接チャンクを設定
void Chunk::SetNeighborChunk(DirectionXYZ direction, Chunk* neighbor)
{
	if (direction == DirectionXYZ::None) return;
	neighbors_[static_cast<size_t>(direction)] = neighbor;

	if (neighbor)
	{
		switch (direction)
		{
		case DirectionXYZ::Left:
		case DirectionXYZ::Right:
		{
			const int32_t haloX = (direction == DirectionXYZ::Left) ? -1 : Constexprs::kChunkBlockCountX;
			const int32_t neighborX = (direction == DirectionXYZ::Left) ? Constexprs::kChunkBlockCountX - 1 : 0;
			for (int32_t y = 0; y < Constexprs::kChunkBlockCountY; ++y)
				for (int32_t z = 0; z < Constexprs::kChunkBlockCountZ; ++z)
					SetBlockIDs(Vector3int(haloX, y, z), neighbor->blocks_[neighborX][y][z]);
			break;
		}
		case DirectionXYZ::Back:
		case DirectionXYZ::Front:
		{
			const int32_t haloZ = (direction == DirectionXYZ::Back) ? -1 : Constexprs::kChunkBlockCountZ;
			const int32_t neighborZ = (direction == DirectionXYZ::Back) ? Constexprs::kChunkBlockCountZ - 1 : 0;
			for (int32_t x = 0; x < Constexprs::kChunkBlockCountX; ++x)
				for (int32_t y = 0; y < Constexprs::kChunkBlockCountY; ++y)
					SetBlockIDs(Vector3int(x, y, haloZ), neighbor->blocks_[x][y][neighborZ]);
			break;
		}
		case DirectionXYZ::Down:
		case DirectionXYZ::Up:
		{
			const int32_t haloY = (direction == DirectionXYZ::Down) ? -1 : Constexprs::kChunkBlockCountY;
			const int32_t neighborY = (direction == DirectionXYZ::Down) ? Constexprs::kChunkBlockCountY - 1 : 0;
			for (int32_t x = 0; x < Constexprs::kChunkBlockCountX; ++x)
				for (int32_t z = 0; z < Constexprs::kChunkBlockCountZ; ++z)
					SetBlockIDs(Vector3int(x, haloY, z), neighbor->blocks_[x][neighborY][z]);
			break;
		}
		default:
			break;
		}
	}
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
	if (blockIdsDirty_) UpdateBlockIds();

	Matrix4x4 viewPro = Game::Camera::Getter::GetViewProjectionMatrix(cameraID);

	uint32_t asHeapSlotTable = Game::Resource::GetSRV(faceCountResourceID_);
	render_->SetCBufferData(0, ShaderType::AmplificationShader, &asHeapSlotTable);
	render_->SetCBufferData(1, ShaderType::AmplificationShader, &chunkInfo_);
	render_->SetCBufferData(2, ShaderType::AmplificationShader, &viewPro);

	Vector4uint msSrvIndexTable = Vector4uint(
		App::Data::Item::GetBlockInfoTableHeapSlot(),
		Game::Resource::GetSRV(bakedFacesResourceID_),
		Game::Resource::GetSRV(faceCountResourceID_),
		Game::Resource::GetSRV(groupOffsetResourceID_));
	render_->SetCBufferData(0, ShaderType::MeshShader, &msSrvIndexTable);
	render_->SetCBufferData(1, ShaderType::MeshShader, &chunkInfo_);
	render_->SetCBufferData(2, ShaderType::MeshShader, &viewPro);
	
	inCamera_ = Game::Camera::InCamera(chunkAABB_, cameraID);
}
void Chunk::UpdateBlockIds()
{
	Game::Resource::UpdateData(blockIDsResourceID_, blockIds_, sizeof(uint32_t), Constexprs::kMaxFacesPerChunkPlusHalo);

	// 面カウンタを0にリセット(このDispatchの中でInterlockedAddの起点にする)
	Game::Resource::ZeroFillCompute(faceWriteCounterResourceID_, sizeof(uint32_t));

	Vector2uint csHeapSlotTable = Vector2uint(
		App::Data::Item::GetBlockInfoTableHeapSlot(),
		Game::Resource::GetSRV(blockIDsResourceID_));
	compute_->SetCBufferData(0, &csHeapSlotTable);

	Vector4uint toCSChunkInfo = Vector4uint(
		Constexprs::kChunkBlockCountX, 
		Constexprs::kChunkBlockCountY, 
		Constexprs::kChunkBlockCountZ, 
		Constexprs::kMaxFacesPerChunk);
	compute_->SetCBufferData(1, &toCSChunkInfo);

	compute_->SetUAVData(0, Game::Resource::GetUAV(bakedFacesResourceID_));
	compute_->SetUAVData(1, Game::Resource::GetUAV(faceCountResourceID_));
	compute_->SetUAVData(2, Game::Resource::GetUAV(faceWriteCounterResourceID_));
	compute_->SetUAVData(3, Game::Resource::GetUAV(groupOffsetResourceID_));
	compute_->Dispatch();

	blockIdsDirty_ = false;
}



void Chunk::Draw(int32_t renderTargetID)
{
	if (inCamera_)
	{
		render_->Draw(renderTargetID);
	}
}

BlockID* Chunk::GetBlockID(const Vector3int& pos)
{
	return &blocks_[pos.x][pos.y][pos.z];
}
BlockID* Chunk::GetBlockIDHelloNeighbor(const Vector3int& index)
{
	// チャンク内
	if (0 <= index.x && index.x < Constexprs::kChunkBlockCountX &&
		0 <= index.y && index.y < Constexprs::kChunkBlockCountY &&
		0 <= index.z && index.z < Constexprs::kChunkBlockCountZ)
	{
		return &blocks_[index.x][index.y][index.z];
	}

	Chunk* targetChunk = nullptr;
	Vector3int localIndex = index;

	if (localIndex.x < 0)
	{
		targetChunk = neighbors_[static_cast<size_t>(DirectionXYZ::Left)];
		if (!targetChunk) return nullptr;
		localIndex.x += Constexprs::kChunkBlockCountX; // -1 -> Constexprs::kChunkBlockCountX-1
	}
	else if (localIndex.x >= Constexprs::kChunkBlockCountX)
	{
		targetChunk = neighbors_[static_cast<size_t>(DirectionXYZ::Right)];
		if (!targetChunk) return nullptr;
		localIndex.x -= Constexprs::kChunkBlockCountX; // Constexprs::kChunkBlockCountX -> 0
	}
	else if (localIndex.z < 0)
	{
		targetChunk = neighbors_[static_cast<size_t>(DirectionXYZ::Back)];
		if (!targetChunk) return nullptr;
		localIndex.z += Constexprs::kChunkBlockCountZ; // -1 -> Constexprs::kChunkBlockCountZ-1
	}
	else if (localIndex.z >= Constexprs::kChunkBlockCountZ)
	{
		targetChunk = neighbors_[static_cast<size_t>(DirectionXYZ::Front)];
		if (!targetChunk) return nullptr;
		localIndex.z -= Constexprs::kChunkBlockCountZ; // Constexprs::kChunkBlockCountZ -> 0
	}
	else if (localIndex.y < 0)
	{
		targetChunk = neighbors_[static_cast<size_t>(DirectionXYZ::Down)];
		if (!targetChunk) return nullptr;
		localIndex.y += Constexprs::kChunkBlockCountY; // -1 -> Constexprs::kChunkBlockCountY-1
	}
	else if (localIndex.y >= Constexprs::kChunkBlockCountY)
	{
		targetChunk = neighbors_[static_cast<size_t>(DirectionXYZ::Up)];
		if (!targetChunk) return nullptr;
		localIndex.y -= Constexprs::kChunkBlockCountY; // Constexprs::kChunkBlockCountY -> 0
	}
	else targetChunk = this;

	return targetChunk->GetBlockID(localIndex);
}

void Chunk::SetBlock(const Vector3int& localIndex, const BlockID id)
{
	// チャンク更新フラグ
	blockIdsDirty_ = true;

	// 3次元配列に入れる
	blocks_[localIndex.x][localIndex.y][localIndex.z] = id;
	// 1次元配列に入れる
	SetBlockIDs(localIndex, id);

	// マップ端のブロックを更新した場合隣接チャンクも更新する
	if (localIndex.x == 0)                       PushToNeighborHalo(DirectionXYZ::Left, localIndex, id);
	if (localIndex.x == Constexprs::kChunkBlockCountX - 1) PushToNeighborHalo(DirectionXYZ::Right, localIndex, id);
	if (localIndex.z == 0)                       PushToNeighborHalo(DirectionXYZ::Back, localIndex, id);
	if (localIndex.z == Constexprs::kChunkBlockCountZ - 1) PushToNeighborHalo(DirectionXYZ::Front, localIndex, id);
	if (localIndex.y == 0)                       PushToNeighborHalo(DirectionXYZ::Down, localIndex, id);
	if (localIndex.y == Constexprs::kChunkBlockCountY - 1) PushToNeighborHalo(DirectionXYZ::Up, localIndex, id);
}
void Chunk::SetBlockIDs(const Vector3int& localIndex, BlockID id)
{
	const int32_t sizeX = Constexprs::kChunkBlockCountX + 2;
	const int32_t sizeY = Constexprs::kChunkBlockCountY + 2;
	const int32_t index = (localIndex.x + 1) + ((localIndex.y + 1) * sizeX) + ((localIndex.z + 1) * sizeX * sizeY);
	blockIds_[index] = static_cast<uint32_t>(id);
	blockIdsDirty_ = true;
}

void Chunk::PushToNeighborHalo(DirectionXYZ direction, const Vector3int& localIndex, BlockID id)
{
	Chunk* neighbor = neighbors_[static_cast<size_t>(direction)];
	if (!neighbor) return;

	// 隣接チャンクから見た時のローカル座標に変換する
	Vector3int neighborLocal = localIndex;
	switch (direction)
	{
	case DirectionXYZ::Left:  neighborLocal.x = Constexprs::kChunkBlockCountX; break;
	case DirectionXYZ::Right: neighborLocal.x = -1;                  break;
	case DirectionXYZ::Back:  neighborLocal.z = Constexprs::kChunkBlockCountZ; break;
	case DirectionXYZ::Front: neighborLocal.z = -1;                  break;
	case DirectionXYZ::Down:  neighborLocal.y = Constexprs::kChunkBlockCountY; break;
	case DirectionXYZ::Up:    neighborLocal.y = -1;                  break;
	default: return;
	}

	neighbor->SetBlockIDs(neighborLocal, id);
}

#include "MapManager.h"
#include <GameObjects/Map/Chunk/Chunk.h>
#include <GameObjects/Map/Chunk/Block/Block.h>
#include <fstream>
#include <sstream>
#include <PerlinNoise.h>
#include <Utilities/functions.h>
#include <filesystem>
#include <algorithm>
#include <limits>

int32_t LocalMod(int32_t a, int32_t n)
{
	return (a % n + n) % n;
}

MapManager::MapManager()
{
	drawRadius_.x = 1;
	drawRadius_.y = 1;
	drawRadius_.z = 1;
	updateRadius_.x = 1;
	updateRadius_.y = 1;
	updateRadius_.z = 1;
}

MapManager::~MapManager(){}

void MapManager::Initialize()
{
	//CreateNewMap(123456);
}


// マップ名とファイルパスの対応表読み込み/保存 (マップ自体のデータは含まれていない)
void MapManager::LoadNameAndPathMap(const std::string& filePath)
{
	std::string csvFilePath = filePath;
	// CSVファイル存在確認
	if (std::filesystem::exists(csvFilePath))
	{
		std::ifstream file(csvFilePath);
		// 開けるか確認
		if (!file.is_open()) return;

		std::string line;
		std::getline(file, line);

		while (std::getline(file, line))
		{
			std::istringstream ss(line);
			std::string name, path;
			std::getline(ss, name, ',');
			std::getline(ss, path, ',');
			mapNameToFilePath_[name] = path;
		}
	}
}
void MapManager::SaveNameAndPathMap(const std::string& filePath)
{
	std::string csvFilePath = filePath;
	std::ofstream file(csvFilePath);
	// 開けるか確認
	if (!file.is_open()) return;
	// ヘッダー行
	file << "MapName,FilePath\n";
	for (const auto& [name, path] : mapNameToFilePath_)
	{
		file << name << "," << path << "\n";
	}
}

// 新規マップ作成
void MapManager::CreateNewMap(const std::string& mapName, uint32_t seed)
{
	// ノイズパラメータ設定
	noiseParam_.seed = seed;			// 俗に言うシード値
	noiseParam_.scale = 32.0f;			// 地形の粗さ（大きくすると緩やか）
	noiseParam_.octaves = 4;			// 反復回数 (大きくすると細かい起伏が増える)
	noiseParam_.persistence = 0.5f;		// 各オクターブの振幅減衰 (大きくすると細かい起伏が増える)
	noiseParam_.height = std::numeric_limits<int32_t>::max();		// マップの高さ
	noiseParam_.pn = PerlinNoise(seed);	// PerlinNoise インスタンス生成

	// mapNameToFilePath_ に新規マップ登録
	mapNameToFilePath_[mapName] = "resources/Minecraft/Maps/" + mapName + ".json";
	currentMapFilePath_ = "resources/Minecraft/Maps/" + mapName + ".json";
}

void MapManager::SetSeed(uint32_t seed)
{
	// ノイズパラメータ設定
	noiseParam_.seed = seed;			// 俗に言うシード値
	noiseParam_.scale = 32.0f;			// 地形の粗さ（大きくすると緩やか）
	noiseParam_.octaves = 4;			// 反復回数 (大きくすると細かい起伏が増える)
	noiseParam_.persistence = 0.5f;		// 各オクターブの振幅減衰 (大きくすると細かい起伏が増える)
	noiseParam_.height = Constexprs::kChunkY;		// マップの高さ
	noiseParam_.pn = PerlinNoise(seed);	// PerlinNoise インスタンス生成
}

// マップ読み込み
void MapManager::LoadMap(const std::string& mapName)
{
	// 相互参照を切る
	for (auto& [pos, chunk] : chunks)
	{
		if (!chunk) continue;
		for (int32_t dir = 0; dir < 4; ++dir)
		{
			chunk->SetNeighborChunk(static_cast<DirectionXYZ>(dir), nullptr);
		}
	}
	// chunks 全解放&clear
	for (auto& pair : chunks)
	{
		pair.second.reset();
	}
	chunks.clear();
	// chunkScheduled_/chunkCreated_ を clear
	chunkScheduled_.clear();
	chunkCreated_.clear();

	// マップネーム保存
	currentMapName_ = mapName;
	// mapNameからファイルパスを取得
	currentMapFilePath_ = mapNameToFilePath_[currentMapName_];

	// JSONから読み込み
	//JsonManager json;
	//json.LoadFromJson(*this, currentMapFilePath_);
}
// マップ保存
void MapManager::SaveMap()
{
	//JsonManager json;
	// JSONへ保存
	//json.SaveToJson(*this, currentMapFilePath_);

	SaveNameAndPathMap("resources/Minecraft/Maps/MapNameAndPath.csv");
}


// チャンク取得、なくても生成はしない
Chunk* MapManager::GetChunk(const Vector3int& chunkPos) const
{
	auto it = chunks.find(chunkPos);
	if (it == chunks.end()) { return nullptr; }
	return it->second.get();
}

// スケジュールに登録
void MapManager::EnsureChunkScheduled(const Vector3int& chunkPos)
{
	// チャンクが既に作成されているならreturn
	if (chunkCreated_.find(chunkPos) != chunkCreated_.end()) return;
	// 既にスケジュール済みならreturn
	if (chunkScheduled_.find(chunkPos) != chunkScheduled_.end()) return;

	// スケジュール済み集合にも登録
	chunkScheduled_.insert(chunkPos);

	//// chunkGenQueue_をプレイヤー位置から近い順にソートする
	//std::vector<Vector3int> tempQueue;
	//while (!chunkGenQueue_.empty())
	//{
	//	tempQueue.push_back(chunkGenQueue_.front());
	//	chunkGenQueue_.pop();
	//}
	//Vector3int playerIndex = ChunkIndexByPosition(player_->data_.translate.value);
	//std::sort(tempQueue.begin(), tempQueue.end(),
	//	[playerIndex](const Vector2int& a, const Vector2int& b)
	//	{
	//		int32_t distA = (a.x - playerIndex.x) * (a.x - playerIndex.x) + (a.y - playerIndex.y) * (a.y - playerIndex.y);
	//		int32_t distB = (b.x - playerIndex.x) * (b.x - playerIndex.x) + (b.y - playerIndex.y) * (b.y - playerIndex.y);
	//		return distA < distB;
	//	});
	//for (const auto& pos : tempQueue)
	//{
	//	chunkGenQueue_.push(pos);
	//}
}

// スケジュールに登録されたチャンクを1つ生成
void MapManager::ProcessChunkGeneration(const Vector3int& cameraChunkPos)
{
	// スケジュールキューが空ではないなら作成
	if (!chunkScheduled_.empty())
	{
		// 近い順にソートする
		std::vector<Vector3int> tempQueue(chunkScheduled_.begin(), chunkScheduled_.end());
		std::sort(tempQueue.begin(), tempQueue.end(),
			[cameraChunkPos](const Vector3int& a, const Vector3int& b)
			{
				int32_t distA = (a.x - cameraChunkPos.x) * (a.x - cameraChunkPos.x) + (a.y - cameraChunkPos.y) * (a.y - cameraChunkPos.y) + (a.z - cameraChunkPos.z) * (a.z - cameraChunkPos.z);
				int32_t distB = (b.x - cameraChunkPos.x) * (b.x - cameraChunkPos.x) + (b.y - cameraChunkPos.y) * (b.y - cameraChunkPos.y) + (b.z - cameraChunkPos.z) * (b.z - cameraChunkPos.z);
				return distA < distB;
			});

		// キューから取り出し
		Vector3int pos = tempQueue.front();
		chunkScheduled_.erase(pos);

		// 一応存在確認
		if (GetChunk(pos) != nullptr)
		{
			// 生成済み集合に登録
			chunkCreated_.insert(pos);
			return;
		}

		// 新規生成
		std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>();
		// チャンクデータ生成
		chunk->CreateChunkData(noiseParam_, pos);

		// 隣接チャンク設定
		static constexpr Vector3int dirOffsets[6] =
		{
			Vector3int(-1, 0, 0),	// Left
			Vector3int(1, 0, 0),	// Right
			Vector3int(0, 0, -1),	// Back
			Vector3int(0, 0, 1),	// Front
			Vector3int(0, -1, 0),	// Down
			Vector3int(0, 1, 0)		// Up
		};

		static constexpr DirectionXYZ dirArr[6] =
		{
			DirectionXYZ::Left,
			DirectionXYZ::Right,
			DirectionXYZ::Back,
			DirectionXYZ::Front,
			DirectionXYZ::Down,
			DirectionXYZ::Up,
		};

		static constexpr DirectionXYZ opposite[6] =
		{
			DirectionXYZ::Right,// Left  の反対
			DirectionXYZ::Left,	// Right の反対
			DirectionXYZ::Front,// Back  の反対
			DirectionXYZ::Back,	// Front の反対
			DirectionXYZ::Up,	// Down  の反対
			DirectionXYZ::Down,	// Up    の反対
		};

		for (int32_t dir = 0; dir < 6; ++dir)
		{
			Chunk* neighbor = GetChunk(pos + dirOffsets[dir]);

			// 自分→隣（隣が無ければnullptrでOK）
			chunk->SetNeighborChunk(dirArr[dir], neighbor);

			// 隣→自分（隣がある時だけ）
			if (neighbor)
			{
				neighbor->SetNeighborChunk(opposite[dir], chunk.get());
			}
		}

		// チャンク登録
		chunks[pos] = std::move(chunk);

		// 生成済み集合に登録
		chunkCreated_.insert(pos);
	}
}


void MapManager::Update(int32_t cameraID)
{
	cameraChunkPos_ = ChunkIndexByPosition(Game::Camera::Getter::GetCenter(cameraID));

	// 1Fに1つのチャンクを作成する
	ProcessChunkGeneration(cameraChunkPos_);

	// プレイヤー周囲更新
	for (int32_t dx = -updateRadius_.x; dx <= updateRadius_.x; ++dx)
	{
		for (int32_t dy = -updateRadius_.y; dy <= updateRadius_.y; ++dy)
		{
			for (int32_t dz = -updateRadius_.z; dz <= updateRadius_.z; ++dz)
			{
				if (cameraChunkPos_.y + dy < 0 || cameraChunkPos_.y + dy >= Constexprs::kChunkStackHeight)
				{
					continue;
				}

				Vector3int chunkPos(cameraChunkPos_.x + dx, cameraChunkPos_.y + dy, cameraChunkPos_.z + dz);
				Chunk* chunk = GetChunk(chunkPos);
				if (chunk) { chunk->Update(cameraID); }
				else EnsureChunkScheduled(chunkPos);
			}
		}
	}
}

void MapManager::Draw(int32_t renderTargetID)
{	
	// プレイヤー周囲描画
	for (int32_t dx = -drawRadius_.x; dx <= drawRadius_.x; ++dx)
	{
		for (int32_t dy = -drawRadius_.y; dy <= drawRadius_.y; ++dy)
		{
			for (int32_t dz = -drawRadius_.z; dz <= drawRadius_.z; ++dz)
			{
				if (cameraChunkPos_.y + dy < 0 || cameraChunkPos_.y + dy >= Constexprs::kChunkStackHeight)
				{
					continue;
				}

				Vector3int chunkPos = Vector3int(cameraChunkPos_.x + dx, cameraChunkPos_.y + dy, cameraChunkPos_.z + dz);
				Chunk* chunk = GetChunk(chunkPos);
				if (chunk) { chunk->Draw(renderTargetID); }
			}
		}
	}
}

void MapManager::DrawImGui()
{
	ImGui::Begin("MapManager");
	ImGui::Text("Chunks count : %zu", chunks.size());
	ImGui::Text("GenerateQueue: %zu", chunkScheduled_.size());
	ImGui::End();
}

//// 指定位置のブロックを破壊
//void MapManager::DestroyBlockAt(const Vector3int& chunkPos, const Vector3int& localIndex)
//{
//	// 破壊するチャンクを取得
//	Chunk* chunk = GetChunk(chunkPos);
//	if (!chunk) return;
//
//	// 破壊するブロックを取得
//	Block* targetBlock = chunk->blocks_[localIndex.x][localIndex.y][localIndex.z].get();
//	if (!targetBlock) return;
//
//	// 破壊するブロックのIDを取得
//	const BlockID destroyedId = targetBlock->GetBlockID();
//	if (destroyedId == BlockID::Air) return;
//
//	// ブロック破壊
//	chunk->DestroyBlock(localIndex);
//
//	// 露出状態更新
//	chunk->SetExposedAroundBlocks(localIndex);
//
//	// ドロップアイテム生成
//	//AddDropItemAt(targetBlock->position_, BlockIdToDropItemId(destroyedId));
//}

//void MapManager::AddDropItemAt(const Vector3& position, ItemID id)
//{
//	//Vector3 dropPos = position;
//	//dropPos.x += Game::Math::RandFloat(-0.3f, 0.3f, 2);
//	//dropPos.z += Game::Math::RandFloat(-0.3f, 0.3f, 2);
//	//dropItemManager_->AddItem(id, dropPos);
//}

// 指定位置にブロックを設置
bool MapManager::SetBlockAt(const Vector3int& chunkPos, const Vector3int& localIndex, BlockID id)
{
	// 設置するチャンクを取得
	Chunk* chunk = GetChunk(chunkPos);
	if (!chunk) return false;

	// 設置するブロック単位の空間を取得
	Block* targetBlock = chunk->GetBlock(localIndex);
	if (!targetBlock) return false;

	// キャラクターと重なってたら設置できない
	const AABB placeAabb = GetAABB(chunkPos, localIndex);
	if (IsOverlappingAnyCharacter(placeAabb)) return false;

	// ブロック設置
	chunk->SetBlock(localIndex, id);

	// 露出状態更新
	chunk->SetExposedAroundBlocks(localIndex);

	return true;
}
bool MapManager::SetBlockAt(const lookAtBlock& lab, BlockID id)
{
	// 向きが不明ならreturn
	if (lab.face == AABBFace::NONE) return false;

	Vector3int offset = Vector3int(0, 0, 0);

	switch (lab.face)
	{
	case AABBFace::XPlus:
		offset.x = -1;
		break;
	case AABBFace::XMinus:
		offset.x = 1;
		break;
	case AABBFace::YMinus:
		offset.y = -1;
		break;
	case AABBFace::YPlus:
		offset.y = 1;
		break;
	case AABBFace::ZMinus:
		offset.z = -1;
		break;
	case AABBFace::ZPlus:
		offset.z = 1;
		break;
	default:
		return false;
		break;
	}

	Vector3int chunkPos = lab.chunkIndex;
	Vector3int localIndex = lab.localIndex + offset;

	// チャンク跨ぎ対応
	if (localIndex.x < 0)
	{
		chunkPos.x -= 1;
		localIndex.x += Constexprs::kChunkX;
	}
	else if (localIndex.x >= Constexprs::kChunkX)
	{
		chunkPos.x += 1;
		localIndex.x -= Constexprs::kChunkX;
	}
	if (localIndex.y < 0)
	{
		chunkPos.y -= 1;
		localIndex.y += Constexprs::kChunkY;
	}
	else if (localIndex.y >= Constexprs::kChunkY)
	{
		chunkPos.y += 1;
		localIndex.y -= Constexprs::kChunkY;
	}
	if (localIndex.z < 0)
	{
		chunkPos.z -= 1;
		localIndex.z += Constexprs::kChunkZ;
	}
	else if (localIndex.z >= Constexprs::kChunkZ)
	{
		chunkPos.z += 1;
		localIndex.z -= Constexprs::kChunkZ;
	}

	return SetBlockAt(chunkPos, localIndex, id);
}
bool MapManager::SetBlockAt(const Vector3& position, BlockID id)
{
	return SetBlockAt(ChunkIndexByPosition(position), BlockIndexByPosition(position), id);
}


void MapManager::SweepAABB(const AABB& aabb, Vector3& movement)
{
	// 何層分判定するか
	Vector3int layerCount = Vector3int(
		static_cast<int32_t>(std::ceil(std::abs(movement.x) / Constexprs::kBlockSize)),
		static_cast<int32_t>(std::ceil(std::abs(movement.y) / Constexprs::kBlockSize)),
		static_cast<int32_t>(std::ceil(std::abs(movement.z) / Constexprs::kBlockSize))
	);

	AABB movedAABB = aabb;;// +movement;

	Vector3int ableToMove = Vector3int(0, 0, 0);

	if (movement.x > 0) ableToMove.x = SweepAABB(movedAABB, AABBFace::XPlus, layerCount.x);
	else if (movement.x < 0) ableToMove.x = SweepAABB(movedAABB, AABBFace::XMinus, layerCount.x);

	if (movement.y > 0) ableToMove.y = SweepAABB(movedAABB, AABBFace::YPlus, layerCount.y);
	else if (movement.y < 0) ableToMove.y = SweepAABB(movedAABB, AABBFace::YMinus, layerCount.y);

	if (movement.z > 0) ableToMove.z = SweepAABB(movedAABB, AABBFace::ZPlus, layerCount.z);
	else if (movement.z < 0) ableToMove.z = SweepAABB(movedAABB, AABBFace::ZMinus, layerCount.z);


	// movement.x == nの時→nブロック分進んでも衝突はしない
	Vector3 wallDist = Vector3(ableToMove.x * Constexprs::kBlockSize, ableToMove.y * Constexprs::kBlockSize, ableToMove.z * Constexprs::kBlockSize);
	movement.x = (movement.x > 0) ? std::min(movement.x, wallDist.x) : std::max(movement.x, -wallDist.x);
	movement.y = (movement.y > 0) ? std::min(movement.y, wallDist.y) : std::max(movement.y, -wallDist.y);
	movement.z = (movement.z > 0) ? std::min(movement.z, wallDist.z) : std::max(movement.z, -wallDist.z);
}
int32_t MapManager::SweepAABB(const AABB& aabb, AABBFace face, int32_t layerCount)
{
	// AABBのサイズから各軸のブロック数を算出
	// 端数や最小値などを考慮し、適宜計算式を調整してください
	int32_t countX = std::max(1, static_cast<int32_t>((aabb.max.x - aabb.min.x) / Constexprs::kBlockSize));
	int32_t countY = std::max(1, static_cast<int32_t>((aabb.max.y - aabb.min.y) / Constexprs::kBlockSize));
	int32_t countZ = std::max(1, static_cast<int32_t>((aabb.max.z - aabb.min.z) / Constexprs::kBlockSize));

	Vector3 basePos;
	Vector3 uDir, vDir, depthDir;
	int32_t countU = 0;
	int32_t countV = 0;

	// 指定された面に応じて基準位置と走査する方向ベクトルを決定
	switch (face)
	{
	case AABBFace::XPlus:
		basePos = Vector3(aabb.max.x, aabb.min.y, aabb.min.z);
		uDir = Vector3(0, 1, 0); countU = countY; // Y軸方向へ走査
		vDir = Vector3(0, 0, 1); countV = countZ; // Z軸方向へ走査
		depthDir = Vector3(1, 0, 0);              // X軸正方向へ深さを測る
		break;

	case AABBFace::XMinus:
		basePos = Vector3(aabb.min.x, aabb.min.y, aabb.min.z);
		uDir = Vector3(0, 1, 0); countU = countY; // Y軸方向へ走査
		vDir = Vector3(0, 0, 1); countV = countZ; // Z軸方向へ走査
		depthDir = Vector3(-1, 0, 0);             // X軸負方向へ深さを測る
		break;

	case AABBFace::YPlus:
		basePos = Vector3(aabb.min.x, aabb.max.y, aabb.min.z);
		uDir = Vector3(1, 0, 0); countU = countX; // X軸方向へ走査
		vDir = Vector3(0, 0, 1); countV = countZ; // Z軸方向へ走査
		depthDir = Vector3(0, 1, 0);              // Y軸正方向へ深さを測る
		break;

	case AABBFace::YMinus:
		basePos = Vector3(aabb.min.x, aabb.min.y, aabb.min.z);
		uDir = Vector3(1, 0, 0); countU = countX; // X軸方向へ走査
		vDir = Vector3(0, 0, 1); countV = countZ; // Z軸方向へ走査
		depthDir = Vector3(0, -1, 0);             // Y軸負方向へ深さを測る
		break;

	case AABBFace::ZPlus:
		basePos = Vector3(aabb.min.x, aabb.min.y, aabb.max.z);
		uDir = Vector3(1, 0, 0); countU = countX; // X軸方向へ走査
		vDir = Vector3(0, 1, 0); countV = countY; // Y軸方向へ走査
		depthDir = Vector3(0, 0, 1);              // Z軸正方向へ深さを測る
		break;

	case AABBFace::ZMinus:
		basePos = Vector3(aabb.min.x, aabb.min.y, aabb.min.z);
		uDir = Vector3(1, 0, 0); countU = countX; // X軸方向へ走査
		vDir = Vector3(0, 1, 0); countV = countY; // Y軸方向へ走査
		depthDir = Vector3(0, 0, -1);             // Z軸負方向へ深さを測る
		break;
	}

	// 決定された軸とブロック数に従って走査
	for (int32_t d = 0; d < layerCount; ++d)
	{
		for (int32_t u = 0; u < countU; ++u)
		{
			for (int32_t v = 0; v < countV; ++v)
			{
				// 基準位置から現在の走査距離を加算してチェック座標を算出
				Vector3 checkPos = basePos
					+ uDir * (u * Constexprs::kBlockSize)
					+ vDir * (v * Constexprs::kBlockSize)
					+ depthDir * (d * Constexprs::kBlockSize);

				// チャンク座標とローカル座標を取得
				Vector3int chunkIndex = ChunkIndexByPosition(checkPos);
				Vector3int localIndex = BlockIndexByPosition(checkPos);

				// チャンクを取得
				Chunk* chunk = GetChunk(chunkIndex);
				if (!chunk) continue;

				// ブロックを取得
				Block* block = chunk->GetBlock(localIndex);
				if (!block) continue;

				// 衝突判定
				if (block->GetBlockID() != BlockID::Air)
				{
					return d;
				}
			}
		}
	}

	return layerCount;
}

bool MapManager::IsSolidAt(const Vector3& position) const
{
	Vector3int chunkPos = ChunkIndexByPosition(position);
	Vector3int index = BlockIndexByPosition(position);
	Chunk* chunk = GetChunk(chunkPos);
	if (chunk)
	{
		Block* block = chunk->GetBlock(index);
		if (block && block->GetBlockID() != BlockID::Air)
		{
			return true;
		}
	}
	return false;
}


bool MapManager::IsOverlappingAnyCharacter(const AABB& aabb) const
{
	//for (BaseCharacter* c : Characters_)
	//{
	//	if (!c) continue;
	//	const AABB& ca = c->data_.aabbs[0];
	//	if (IsOverLap(aabb, ca))
	//		return true;
	//}
	return false;
}

AABB MapManager::GetAABB(const Vector3int& chunkPos, const Vector3int& index) const
{
	// チャンクのワールド原点
	float chunkWorldX = chunkPos.x * Constexprs::kChunkX * Constexprs::kBlockSize;
	float chunkWorldY = chunkPos.y * Constexprs::kChunkY * Constexprs::kBlockSize;
	float chunkWorldZ = chunkPos.z * Constexprs::kChunkZ * Constexprs::kBlockSize;

	// ブロックのワールド座標
	float worldX = chunkWorldX + index.x * Constexprs::kBlockSize;
	float worldY = chunkWorldY + index.y * Constexprs::kBlockSize;
	float worldZ = chunkWorldZ + index.z * Constexprs::kBlockSize;

	Vector3 mint(worldX, worldY, worldZ);
	Vector3 maxt(worldX + Constexprs::kBlockSize, worldY + Constexprs::kBlockSize, worldZ + Constexprs::kBlockSize);

	return AABB(mint, maxt);
}
AABB MapManager::GetAABB(const Vector3& position) const
{
	Vector3int chunkPos = ChunkIndexByPosition(position);
	Vector3int index = BlockIndexByPosition(position);
	return GetAABB(chunkPos, index);
}
bool MapManager::GetIsActive(const Vector3int& chunkPos, const Vector3int& index) const
{
	Chunk* chunk = GetChunk(chunkPos);
	if (chunk)
	{
		if (chunk->GetBlock(index)->blockInfo_.ID != BlockID::Air)
		{
			return true;
		}
	}
	return false;
}
bool MapManager::GetIsActive(const Vector3& position) const
{
	Vector3int chunkPos = ChunkIndexByPosition(position);
	Vector3int index = BlockIndexByPosition(position);
	return GetIsActive(chunkPos, index);
}

// position が今どのチャンクに属しているか  例：position=(34, 0, 50) -> chunkIndex=(1, 2)
Vector3int MapManager::ChunkIndexByPosition(const Vector3& position) const
{
	int32_t bx = static_cast<int32_t>(std::floor(position.x / Constexprs::kBlockSize));
	int32_t by = static_cast<int32_t>(std::floor(position.y / Constexprs::kBlockSize));
	int32_t bz = static_cast<int32_t>(std::floor(position.z / Constexprs::kBlockSize));

	Vector3int chunk;
	chunk.x = static_cast<int32_t>(std::floor((float)bx / Constexprs::kChunkX));
	chunk.y = static_cast<int32_t>(std::floor((float)by / Constexprs::kChunkY));
	chunk.z = static_cast<int32_t>(std::floor((float)bz / Constexprs::kChunkZ));
	return chunk;
}

// position が今チャンク内どのブロックに属しているか(どんな時も0～CHUNK_SIZE-1の範囲に収まる)  例：position=(34, 0, 50) -> localIndex=(2, 0, 2)
Vector3int MapManager::BlockIndexByPosition(const Vector3& position) const
{
	Vector3int wb = WorldBlockIndexByPosition(position);

	Vector3int local;
	local.x = LocalMod(wb.x, Constexprs::kChunkX);
	local.y = LocalMod(wb.y, Constexprs::kChunkY);
	local.z = LocalMod(wb.z, Constexprs::kChunkZ);

	local.y = std::clamp(local.y, 0, Constexprs::kChunkY - 1);

	return local;
}

Vector3int MapManager::WorldBlockIndexByPosition(const Vector3& position) const
{
	int32_t bx = static_cast<int32_t>(std::floor(position.x / Constexprs::kBlockSize));
	int32_t by = static_cast<int32_t>(std::floor(position.y / Constexprs::kBlockSize));
	int32_t bz = static_cast<int32_t>(std::floor(position.z / Constexprs::kBlockSize));
	return Vector3int(bx, by, bz);
}


// レイとブロックの交差判定（衝突ブロックを返す）
std::optional<lookAtBlock> MapManager::GetBlockByCrossedRay(const Ray& ray, const float maxDistance) const
{
	lookAtBlock result;

	const Vector3 rayStart = ray.origin;
	const Vector3 dir = ray.diff;
	const float rayLen = maxDistance;
	const Vector3 rayEnd = ray.origin + ray.diff * rayLen;

	auto WorldBlockIndexByPosition = [](const Vector3& p) -> Vector3int
		{
			return Vector3int(
				int32_t(std::floor(p.x / Constexprs::kBlockSize)),
				int32_t(std::floor(p.y / Constexprs::kBlockSize)),
				int32_t(std::floor(p.z / Constexprs::kBlockSize))
			);
		};

	auto WorldChunkIndexFromWorldBlock = [](const Vector3int& wb) -> Vector3int
		{
			Vector3int cp;
			cp.x = int32_t(std::floor(float(wb.x) / float(Constexprs::kChunkX)));
			cp.y = int32_t(std::floor(float(wb.y) / float(Constexprs::kChunkY)));
			cp.z = int32_t(std::floor(float(wb.z) / float(Constexprs::kChunkZ)));
			return cp;
		};

	auto Sign = [](float v) -> int32_t { return (v > 0.0f) - (v < 0.0f); };

	auto LocalMod = [](int32_t a, int32_t n) -> int32_t { return (a % n + n) % n; };

	auto LocalIndexFromWorldBlock = [&](const Vector3int& wb) -> Vector3int
		{
			return Vector3int(
				LocalMod(wb.x, Constexprs::kChunkX),
				LocalMod(wb.y, Constexprs::kChunkY),
				LocalMod(wb.z, Constexprs::kChunkZ)
			);
		};

	// sideDist（次の境界までの距離 t）
	auto NextBoundaryT = [](float origin, float dir, int32_t cell, int32_t step) -> float
		{
			// cell = floor(origin/Constexprs::kBlockSize) のブロック座標
			if (step > 0)
			{
				const float next = (float(cell) + 1.0f) * Constexprs::kBlockSize;
				return (next - origin) / dir;
			}
			else
			{
				const float next = float(cell) * Constexprs::kBlockSize;
				return (next - origin) / dir;
			}
		};

	// 現在のワールドブロックインデックス
	Vector3int currentWB = WorldBlockIndexByPosition(rayStart);
	const Vector3int endWB = WorldBlockIndexByPosition(rayEnd);

	// step（dir==0 の軸は 0）
	const int32_t stepX = Sign(dir.x);
	const int32_t stepY = Sign(dir.y);
	const int32_t stepZ = Sign(dir.z);

	// deltaDist（dir==0 は INF 扱い）
	//const float INF = std::numeric_limits<float>::infinity();
	const float INF = 3.402823466e+38F;
	const float deltaDistX = (std::abs(dir.x) < 1e-6f) ? INF : std::abs(Constexprs::kBlockSize / dir.x);
	const float deltaDistY = (std::abs(dir.y) < 1e-6f) ? INF : std::abs(Constexprs::kBlockSize / dir.y);
	const float deltaDistZ = (std::abs(dir.z) < 1e-6f) ? INF : std::abs(Constexprs::kBlockSize / dir.z);

	float sideDistX = (stepX == 0) ? INF : NextBoundaryT(rayStart.x, dir.x, currentWB.x, stepX);
	float sideDistY = (stepY == 0) ? INF : NextBoundaryT(rayStart.y, dir.y, currentWB.y, stepY);
	float sideDistZ = (stepZ == 0) ? INF : NextBoundaryT(rayStart.z, dir.z, currentWB.z, stepZ);

	// 最大ステップ
	const int32_t maxSteps = 2048;

	// 直前に跨いだ面（＝ currentWB に入った面）
	AABBFace enterFace = AABBFace::NONE;


	for (int32_t i = 0; i < maxSteps; ++i)
	{
		// currentWB -> chunk/local
		const Vector3int chunkPos = WorldChunkIndexFromWorldBlock(currentWB);
		const Vector3int local = LocalIndexFromWorldBlock(currentWB);

		Chunk* chunk = GetChunk(chunkPos);
		if (chunk)
		{
			// local.y は縦制限があるので範囲チェック
			if (0 <= local.x && local.x < Constexprs::kChunkX &&
				0 <= local.y && local.y < Constexprs::kChunkY &&
				0 <= local.z && local.z < Constexprs::kChunkZ)
			{
				Block* b = chunk->GetBlock(local);
				if (b && b->GetBlockID() != BlockID::Air)
				{
					const AABB& aabb = b->aabb_;

					result.block = b;
					result.chunkIndex = chunkPos;
					result.localIndex = local;
					result.face = enterFace;

					// 「ブロック中心まで」ではなく「侵入面まで」の距離を返す
					// 今回のSweepは軸レイ(±1,0,0等)なのでこの計算でOK
					float dist = 0.0f;

					switch (enterFace)
					{
					case AABBFace::XPlus:   dist = std::abs(aabb.min.x - rayStart.x); break;
					case AABBFace::XMinus:  dist = std::abs(aabb.max.x - rayStart.x); break;
					case AABBFace::YMinus: dist = std::abs(aabb.min.y - rayStart.y); break;
					case AABBFace::YPlus:    dist = std::abs(aabb.max.y - rayStart.y); break;
					case AABBFace::ZMinus:   dist = std::abs(aabb.min.z - rayStart.z); break;
					case AABBFace::ZPlus:  dist = std::abs(aabb.max.z - rayStart.z); break;
					default:
						// startが既にブロック内などのケース
						dist = 0.0f;
						break;
					}

					result.distance = dist;
					return result;
				}
			}
		}

		// 終点ブロックまで到達したら終了
		if (currentWB == endWB) return std::nullopt;

		// 次に跨ぐ境界（最小のsideDist）を選ぶ
		if (sideDistX < sideDistY)
		{
			if (sideDistX < sideDistZ)
			{
				// X方向へ進む
				currentWB.x += stepX;

				// X方向に進む場合、入ってきた面は stepX の反対側
				// stepX=+1 なら “LEFT面から入る”、stepX=-1 なら “RIGHT面から入る”
				enterFace = (stepX > 0) ? AABBFace::XPlus : AABBFace::XMinus;

				sideDistX += deltaDistX;
			}
			else
			{
				// Z方向へ進む
				currentWB.z += stepZ;

				// stepZ=+1 なら “BACK面から入る”、stepZ=-1 なら “FRONT面から入る”
				enterFace = (stepZ > 0) ? AABBFace::ZMinus : AABBFace::ZPlus;

				sideDistZ += deltaDistZ;
			}
		}
		else
		{
			if (sideDistY < sideDistZ)
			{
				// Y方向へ進む
				currentWB.y += stepY;

				// stepY=+1 なら “BOTTOM面から入る”、stepY=-1 なら “TOP面から入る”
				enterFace = (stepY > 0) ? AABBFace::YMinus : AABBFace::YPlus;

				sideDistY += deltaDistY;
			}
			else
			{
				// Z方向へ進む
				currentWB.z += stepZ;

				enterFace = (stepZ > 0) ? AABBFace::ZMinus : AABBFace::ZPlus;

				sideDistZ += deltaDistZ;
			}
		}

		// 伸びすぎ防止：レイ長を超えたら終了（tの近似として最小sideDistを使う）
		const float tApprox = std::min(sideDistX, std::min(sideDistY, sideDistZ));
		if (tApprox > rayLen) return std::nullopt;
	}

	return std::nullopt;
}

RayHitResult MapManager::GetFirstHitByRay(const Ray& ray, float maxDistance, const BaseCharacter* ignore) const
{
	RayHitResult best{};
	//
	//// 1) ブロック
	//if (auto b = GetBlockByCrossedRay(ray, maxDistance); b.has_value())
	//{
	//	best.type = RayHitResult::Type::Block;
	//	best.blockHit = b.value();
	//	best.distance = b->distance;
	//}
	//
	//// 2) キャラ（登録済みキャラのAABBと判定）
	//float bestCharDist = std::numeric_limits<float>::infinity();
	//BaseCharacter* bestChar = nullptr;
	//
	//// ※ Characters_ を保持している前提（前の実装で追加）
	//for (BaseCharacter* c : Characters_)
	//{
	//	if (!c) continue;
	//	if (c == ignore) continue;
	//
	//	// 自分自身を除外したい場合は呼び出し側で ray.origin の所有者を渡す仕組みが必要。
	//	// ここでは「Rayの原点がそのキャラAABB内なら無視」程度で回避する。
	//	const AABB& ca = c->data_.aabbs[0];
	//
	//	float t = 0.0f;
	//	if (!RayIntersectAABB_FirstT(ray, ca, t)) continue;
	//	if (t < 0.0f) continue;
	//
	//	const float dist = t; // ray.diff が正規化されている前提。Player/Enemyでは Normalized() している。
	//	if (dist <= maxDistance && dist < bestCharDist)
	//	{
	//		bestCharDist = dist;
	//		bestChar = c;
	//	}
	//}
	//
	//if (bestChar)
	//{
	//	// ブロックより手前ならキャラ優先
	//	if (best.type == RayHitResult::Type::None || bestCharDist < best.distance)
	//	{
	//		best.type = RayHitResult::Type::Character;
	//		best.Character = bestChar;
	//		best.distance = bestCharDist;
	//	}
	//}
	//
	return best;
}
// レイとブロックの交差判定（衝突座標を返す）
std::optional<Vector3> MapManager::GetPositionByCrossedRay(const Ray& ray) const
{
	AABB aabb = GetAABB(ray.origin);

	// まずAABBで大まかに判定
	if (!IsCollision(ray, aabb))
	{
		return std::nullopt;
	}

	// AABBに当たっていた場合のみ、三角形ごとに詳細判定 最近衝突点を返す
	std::optional<Vector3> closestPoint = std::nullopt;
	float closestDist = std::numeric_limits<float>::infinity();

	// AABBの各面を構成する三角形を取得
	Triangle triangles[12];
	triangles[0] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.min.z}};
	triangles[1] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.min.z},
		Vector3{aabb.min.x, aabb.max.y, aabb.min.z}};
	triangles[2] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.max.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.max.z},
		Vector3{aabb.max.x, aabb.min.y, aabb.max.z}};
	triangles[3] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.max.z},
		Vector3{aabb.min.x, aabb.max.y, aabb.max.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.max.z}};
	triangles[4] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.min.x, aabb.min.y, aabb.max.z},
		Vector3{aabb.min.x, aabb.max.y, aabb.max.z}};
	triangles[5] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.min.x, aabb.max.y, aabb.max.z},
		Vector3{aabb.min.x, aabb.max.y, aabb.min.z}};
	triangles[6] = Triangle{
		Vector3{aabb.max.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.max.z}};
	triangles[7] = Triangle{
		Vector3{aabb.max.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.max.z},
		Vector3{aabb.max.x, aabb.min.y, aabb.max.z}};
	triangles[8] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.min.y, aabb.max.z}};
	triangles[9] = Triangle{
		Vector3{aabb.min.x, aabb.min.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.min.y, aabb.max.z},
		Vector3{aabb.min.x, aabb.min.y, aabb.max.z}};
	triangles[10] = Triangle{
		Vector3{aabb.min.x, aabb.max.y, aabb.min.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.max.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.min.z}};
	triangles[11] = Triangle{
		Vector3{aabb.min.x, aabb.max.y, aabb.min.z},
		Vector3{aabb.min.x, aabb.max.y, aabb.max.z},
		Vector3{aabb.max.x, aabb.max.y, aabb.max.z}};
	for (const Triangle& t : triangles)
	{
		std::optional<Vector3> pos = IntersectRayTriangle(ray, t);
		if (pos != std::nullopt)
		{
			// 衝突点までの距離を計算
			float dist = (pos.value() - ray.origin).Length();
			if (dist < closestDist)
			{
				closestDist = dist;
				closestPoint = pos;
			}
		}
	}

	return closestPoint;
}

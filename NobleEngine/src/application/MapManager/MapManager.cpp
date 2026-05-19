#include "MapManager/MapManager.h"
#include "MapManager/Chunk/Chunk.h"
#include "MapManager/Chunk/Block/Block.h"
#include "MapManager/Chunk/Block/BlockDurability.h"
#include "Item/DropItem/DropItemManager.h"
#include <fstream>
#include <sstream>
#include "Character/Player/Player.h"
#include "Character/BaseCharacter.h"
#include "Utilities/PerlinNoise.h"
#include "Engine.h"


// AABBの各種補助 いずれstruct AABBに移す

int LocalMod(int a, int n)
{
	return (a % n + n) % n;
}

namespace
{
	static float ClampFloat(float v, float a, float b) { return my_max(a, my_min(v, b)); }

	static float GetAABBSizeX(const AABB& a) { return a.max.x - a.min.x; }
	static float GetAABBSizeY(const AABB& a) { return a.max.y - a.min.y; }
	static float GetAABBSizeZ(const AABB& a) { return a.max.z - a.min.z; }

	// 進行方向に応じて、AABBの「面上のサンプル点」を3x3で作る（簡易）
	static void MakeFaceSamplePoints_X(const AABB& aabb, float xFace, Vector3 outPts[9])
	{
		const float inset = 0.02f;
		const float y0 = aabb.min.y + inset;
		const float y1 = (aabb.min.y + aabb.max.y) * 0.5f;
		const float y2 = aabb.max.y - inset;

		const float z0 = aabb.min.z + inset;
		const float z1 = (aabb.min.z + aabb.max.z) * 0.5f;
		const float z2 = aabb.max.z - inset;

		outPts[0] = { xFace, y0, z0 };
		outPts[1] = { xFace, y0, z1 };
		outPts[2] = { xFace, y0, z2 };
		outPts[3] = { xFace, y1, z0 };
		outPts[4] = { xFace, y1, z1 };
		outPts[5] = { xFace, y1, z2 };
		outPts[6] = { xFace, y2, z0 };
		outPts[7] = { xFace, y2, z1 };
		outPts[8] = { xFace, y2, z2 };
	}

	static void MakeFaceSamplePoints_Y(const AABB& aabb, float yFace, Vector3 outPts[9])
	{
		const float inset = 0.02f;
		const float x0 = aabb.min.x + inset;
		const float x1 = (aabb.min.x + aabb.max.x) * 0.5f;
		const float x2 = aabb.max.x - inset;

		const float z0 = aabb.min.z + inset;
		const float z1 = (aabb.min.z + aabb.max.z) * 0.5f;
		const float z2 = aabb.max.z - inset;

		outPts[0] = { x0, yFace, z0 };
		outPts[1] = { x1, yFace, z0 };
		outPts[2] = { x2, yFace, z0 };
		outPts[3] = { x0, yFace, z1 };
		outPts[4] = { x1, yFace, z1 };
		outPts[5] = { x2, yFace, z1 };
		outPts[6] = { x0, yFace, z2 };
		outPts[7] = { x1, yFace, z2 };
		outPts[8] = { x2, yFace, z2 };
	}

	static void MakeFaceSamplePoints_Z(const AABB& aabb, float zFace, Vector3 outPts[9])
	{
		const float inset = 0.02f;
		const float x0 = aabb.min.x + inset;
		const float x1 = (aabb.min.x + aabb.max.x) * 0.5f;
		const float x2 = aabb.max.x - inset;

		const float y0 = aabb.min.y + inset;
		const float y1 = (aabb.min.y + aabb.max.y) * 0.5f;
		const float y2 = aabb.max.y - inset;

		outPts[0] = { x0, y0, zFace };
		outPts[1] = { x1, y0, zFace };
		outPts[2] = { x2, y0, zFace };
		outPts[3] = { x0, y1, zFace };
		outPts[4] = { x1, y1, zFace };
		outPts[5] = { x2, y1, zFace };
		outPts[6] = { x0, y2, zFace };
		outPts[7] = { x1, y2, zFace };
		outPts[8] = { x2, y2, zFace };
	}
}

namespace
{
	// レイ vs AABB（スラブ法）：交差したら、最初に入る距離tを返す
	static bool RayIntersectAABB_FirstT(const Ray& ray, const AABB& aabb, float& outT)
	{
		const float INF = std::numeric_limits<float>::infinity();
		float tmin = 0.0f;
		float tmax = INF;

		auto Axis = [&](float origin, float dir, float minB, float maxB) -> bool
			{
				if (std::abs(dir) < 1e-6f)
				{
					// 平行：範囲外なら衝突なし
					return (minB <= origin && origin <= maxB);
				}

				const float inv = 1.0f / dir;
				float t1 = (minB - origin) * inv;
				float t2 = (maxB - origin) * inv;
				if (t1 > t2) std::swap(t1, t2);

				tmin = my_max(tmin, t1);
				tmax = my_min(tmax, t2);
				return tmin <= tmax;
			};

		if (!Axis(ray.origin.x, ray.diff.x, aabb.min.x, aabb.max.x)) return false;
		if (!Axis(ray.origin.y, ray.diff.y, aabb.min.y, aabb.max.y)) return false;
		if (!Axis(ray.origin.z, ray.diff.z, aabb.min.z, aabb.max.z)) return false;

		outT = tmin;
		return true;
	}
}

namespace
{
	struct SweepHit
	{
		bool hit = false;
		float t = 1.0f;            // 0..1
		Vector3 normal{ 0,0,0 };   // 当たった面法線（簡易）
	};

	static SweepHit SweptAABBvsAABB(const AABB& moving, const Vector3& delta, const AABB& block)
	{
		// delta を 0..1 のパラメータで扱う
		const float INF = std::numeric_limits<float>::infinity();

		auto Axis = [&](float minA, float maxA, float minB, float maxB, float v, float& tEnter, float& tExit, float& nOut)
			{
				if (std::abs(v) < 1e-8f)
				{
					// 動かない軸：開始時に重なってなければ永遠に当たらない
					if (maxA <= minB || minA >= maxB) { tEnter = INF; tExit = -INF; return; }
					// 重なっているなら制約なし
					return;
				}

				const float invV = 1.0f / v;
				float t1 = (minB - maxA) * invV;
				float t2 = (maxB - minA) * invV;

				float n = 0.0f;
				// t1 が entry 側
				if (t1 > t2) { std::swap(t1, t2); n = (v > 0) ? -1.0f : +1.0f; }
				else { n = (v > 0) ? -1.0f : +1.0f; }

				// entryの法線は「entryで跨ぐ側」だが、簡易に vの符号で決める
				// （厳密にするなら、entry側(t1)が採用された軸の法線にする）
				// ここでは nOut は後段で「採用軸」に上書きするので仮でOK
				nOut = n;

				tEnter = t1;
				tExit = t2;
			};

		float tEnterX = -INF, tExitX = INF; float nx = 0;
		float tEnterY = -INF, tExitY = INF; float ny = 0;
		float tEnterZ = -INF, tExitZ = INF; float nz = 0;

		Axis(moving.min.x, moving.max.x, block.min.x, block.max.x, delta.x, tEnterX, tExitX, nx);
		Axis(moving.min.y, moving.max.y, block.min.y, block.max.y, delta.y, tEnterY, tExitY, ny);
		Axis(moving.min.z, moving.max.z, block.min.z, block.max.z, delta.z, tEnterZ, tExitZ, nz);

		float tEnter = my_max(tEnterX, my_max(tEnterY, tEnterZ));
		float tExit = my_min(tExitX, my_min(tExitY, tExitZ));

		SweepHit out;

		// 交差条件
		if (tEnter > tExit) return out;
		if (tExit < 0.0f) return out;     // 過去に当たった
		if (tEnter > 1.0f) return out;    // 移動範囲外

		out.hit = true;
		out.t = std::clamp(tEnter, 0.0f, 1.0f);

		// 採用軸の法線（tEnterが最大だった軸）
		if (tEnter == tEnterX) out.normal = Vector3{ nx, 0, 0 };
		else if (tEnter == tEnterY) out.normal = Vector3{ 0, ny, 0 };
		else out.normal = Vector3{ 0, 0, nz };

		return out;
	}
}

namespace
{
	struct SolidBlockAABB
	{
		AABB aabb;
	};

	// 負数対応の床除算
	static int FloorDivInt(int a, int n)
	{
		int q = a / n;
		int r = a % n;
		if (r != 0 && ((a < 0) ^ (n < 0))) --q;
		return q;
	}

	// test AABBが覆う範囲の「固体ブロックAABB」を列挙
	template<class Fn>
	static void ForEachSolidBlockAABB_OverlappingRange(
		const MapManager* self,
		const AABB& test,
		Fn&& fn)
	{
		const Vector3int minWB = self->WorldBlockIndexByPosition(test.min);
		const Vector3int maxWB = self->WorldBlockIndexByPosition(test.max - Vector3{ 1e-6f, 1e-6f, 1e-6f });

		for (int by = minWB.y; by <= maxWB.y; ++by)
		{
			if (by < 0 || by >= CHUNK_Y) continue;
			const int localY = by;

			for (int bz = minWB.z; bz <= maxWB.z; ++bz)
			{
				const int chunkZ = FloorDivInt(bz, CHUNK_Z);
				const int localZ = LocalMod(bz, CHUNK_Z);

				for (int bx = minWB.x; bx <= maxWB.x; ++bx)
				{
					const int chunkX = FloorDivInt(bx, CHUNK_X);
					const int localX = LocalMod(bx, CHUNK_X);

					const Vector2int chunkPos{ chunkX, chunkZ };
					Chunk* c = self->TryGetChunk(chunkPos);
					if (!c) continue;

					Block* b = c->blocks_[localX][localY][localZ].get();
					if (!b) continue;
					if (b->GetBlockID() == BlockID::Air) continue;

					fn(b->aabb_);
				}
			}
		}
	}

	static bool AnySolidOverlap_Map(
		const MapManager* self,
		const AABB& test)
	{
		bool hit = false;
		ForEachSolidBlockAABB_OverlappingRange(self, test, [&](const AABB& block)
			{
				if (IsOverLap(test, block))
				{
					hit = true;
				}
			});
		return hit;
	}
}


MapManager::MapManager()
{
	dropItemManager_ = std::make_unique<DropItemManager>();
	dropItemManager_->SetMapManager(this);
}

void MapManager::SetPlayer(Player* player)
{
	// プレイヤー参照保存
	player_ = player;

	RegisterCharacter(player_);
	dropItemManager_->SetPlayer(player);
}

MapManager::~MapManager(){}

void MapManager::Initialize()
{
	//CreateNewMap(123456);
}


// マップ名とファイルパスの対応表読み込み/保存
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
	noiseParam_.height = CHUNK_Y;		// マップの高さ
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
	noiseParam_.height = CHUNK_Y;		// マップの高さ
	noiseParam_.pn = PerlinNoise(seed);	// PerlinNoise インスタンス生成
}

// マップ読み込み
void MapManager::LoadMap(const std::string& mapName)
{
	// 相互参照を切る
	for (auto& [pos, chunk] : chunks)
	{
		if (!chunk) continue;
		for (int dir = 0; dir < 4; ++dir)
		{
			chunk->SetNeighborChunk(static_cast<DirectionXZ>(dir), nullptr);
		}
	}
	// chunks 全解放&clear
	for (auto& pair : chunks)
	{
		pair.second.reset();
	}
	chunks.clear();
	// chunkGenQueue_ を空に
	while (!chunkGenQueue_.empty())
	{
		chunkGenQueue_.pop();
	}
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

	player_->data_.translate.value.y = 20.0f;
	player_->data_.translate.velocity.y = 0.0f;
	player_->data_.translate.acceleration.y = GRAVITY;
}
// マップ保存
void MapManager::SaveMap()
{
	//JsonManager json;
	// JSONへ保存
	//json.SaveToJson(*this, currentMapFilePath_);

	SaveNameAndPathMap("resources/Minecraft/Maps/MapNameAndPath.csv");
}

// チャンク有無確認
bool MapManager::HasChunk(const Vector2int& chunkPos) const
{
	return chunks.find(chunkPos) != chunks.end();
}

// チャンク取得、なくても生成はしない
Chunk* MapManager::TryGetChunk(const Vector2int& chunkPos) const
{
	auto it = chunks.find(chunkPos);
	if (it == chunks.end()) { return nullptr; }
	return it->second.get();
}

// チャンク取得、なければスケジュールに登録して生成
Chunk* MapManager::GetOrCreateChunk(const Vector2int& chunkPos)
{
	// chunkCreated_から既に生成済みか確認
	if (chunkCreated_.find(chunkPos) != chunkCreated_.end())
	{
		return TryGetChunk(chunkPos);
	}


	// 欲しいチャンクが存在しなければスケジュールに登録
	EnsureChunkScheduled(chunkPos);
	// スケジュールに登録されたチャンクを1つ生成
	ProcessChunkGeneration();

	return TryGetChunk(chunkPos);
}

// 欲しいチャンクが存在しなければスケジュールに登録
void MapManager::EnsureChunkScheduled(const Vector2int& chunkPos)
{
	// チャンクが既に作成されているならreturn
	if (chunkCreated_.find(chunkPos) != chunkCreated_.end()) return;
	// 既にスケジュール済みならreturn
	if (chunkScheduled_.find(chunkPos) != chunkScheduled_.end()) return;
	// スケジュール登録
	chunkGenQueue_.push(chunkPos);
	// スケジュール済み集合にも登録
	chunkScheduled_.insert(chunkPos);

	// chunkGenQueue_をプレイヤー位置から近い順にソートする
	std::vector<Vector2int> tempQueue;
	while (!chunkGenQueue_.empty())
	{
		tempQueue.push_back(chunkGenQueue_.front());
		chunkGenQueue_.pop();
	}
	Vector2int playerIndex = ChunkIndexByPosition(player_->data_.translate.value);
	std::sort(tempQueue.begin(), tempQueue.end(),
		[playerIndex](const Vector2int& a, const Vector2int& b)
		{
			int distA = (a.x - playerIndex.x) * (a.x - playerIndex.x) + (a.y - playerIndex.y) * (a.y - playerIndex.y);
			int distB = (b.x - playerIndex.x) * (b.x - playerIndex.x) + (b.y - playerIndex.y) * (b.y - playerIndex.y);
			return distA < distB;
		});
	for (const auto& pos : tempQueue)
	{
		chunkGenQueue_.push(pos);
	}
}

// スケジュールに登録されたチャンクを1つ生成
void MapManager::ProcessChunkGeneration()
{
	// スケジュールキューが空ではないなら作成
	if (!chunkGenQueue_.empty())
	{
		// キューから取り出し
		Vector2int pos = chunkGenQueue_.front();
		chunkGenQueue_.pop();
		chunkScheduled_.erase(pos);

		bool exists = HasChunk(pos);
		// 既にあるなら取得、ないなら新規生成
		std::unique_ptr<Chunk> chunk = exists ? std::move(chunks[pos]) : std::make_unique<Chunk>();
		// チャンクデータ生成
		chunk->CreateChunkData(noiseParam_, pos);
		// チャンク登録
		chunks[pos] = std::move(chunk);


		// 隣接チャンク設定
		static constexpr Vector2int dirOffsets[4] = 
			{
			Vector2int(-1, 0),	// Left
			Vector2int(1, 0),	// Right
			Vector2int(0, -1),	// Back
			Vector2int(0, 1)	// Front
		};

		static constexpr DirectionXZ opposite[4] =
		{
			DirectionXZ::Right,	// Left  の反対
			DirectionXZ::Left,	// Right の反対
			DirectionXZ::Front,	// Back  の反対
			DirectionXZ::Back	// Front の反対
		};

		for (int dir = 0; dir < 4; ++dir)
		{
			Chunk* neighbor = TryGetChunk(pos + dirOffsets[dir]);

			// 自分→隣（隣が無ければnullptrでOK）
			chunk->SetNeighborChunk(static_cast<DirectionXZ>(dir), neighbor);

			// 隣→自分（隣がある時だけ）
			if (neighbor)
			{
				neighbor->SetNeighborChunk(opposite[dir], chunk.get());
			}
		}

		// 生成済み集合に登録
		chunkCreated_.insert(pos);
	}
}


void MapManager::Update()
{
	// デバッグ用：マップロード/セーブ
	//if (Game::IO::Key::IsJustPressed(DIK_0))
	//{
	//	LoadMap(mapFilePath_);
	//}
	//if (Game::IO::Key::IsJustPressed(DIK_1))
	//{
	//	SaveMap(mapFilePath_);
	//}

	// 生成を段階的に実行
	ProcessChunkGeneration();

	// プレイヤー周囲更新
	Vector2int playerIndex = ChunkIndexByPosition(player_->data_.translate.value);
	for (int32_t dx = -updateRadius_; dx <= updateRadius_; ++dx)
	{
		for (int32_t dz = -updateRadius_; dz <= updateRadius_; ++dz)
		{
			Vector2int pos(playerIndex.x + dx, playerIndex.y + dz);
			Chunk* chunk = TryGetChunk(pos);
			if (chunk) { chunk->Update(); }
		}
	}

	// ドロップアイテム更新
	dropItemManager_->Update();
}

void MapManager::Draw()
{
	Vector2int drawCenter = ChunkIndexByPosition(player_->data_.translate.value);

	// プレイヤー周囲描画
	for (int32_t dx = -drawRadius_; dx <= drawRadius_; ++dx)
	{
		for (int32_t dz = -drawRadius_; dz <= drawRadius_; ++dz)
		{
			Vector2int chunkPos = Vector2int(drawCenter.x + dx, drawCenter.y + dz);
			EnsureChunkScheduled(chunkPos);
			Chunk* chunk = TryGetChunk(chunkPos);
			if (chunk) { chunk->Draw(); }
		}
	}

	// ドロップアイテム描画
	dropItemManager_->Draw();
}

void MapManager::DrawImGui()
{
	ImGui::Begin("MapManager");
	ImGui::Text("Chunks count: %zu", chunks.size());
	ImGui::Text("Chunk Generation Queue size: %zu", chunkGenQueue_.size());
	ImGui::End();
}

// 指定位置のブロックを破壊
void MapManager::DestroyBlockAt(const Vector2int& chunkPos, const Vector3int& localIndex)
{
	// 破壊するチャンクを取得
	Chunk* chunk = TryGetChunk(chunkPos);
	if (!chunk) return;

	// 破壊するブロックを取得
	Block* targetBlock = chunk->blocks_[localIndex.x][localIndex.y][localIndex.z].get();
	if (!targetBlock) return;

	// 破壊するブロックのIDを取得
	const BlockID destroyedId = targetBlock->GetBlockID();
	if (destroyedId == BlockID::Air) return;

	// ブロック破壊
	chunk->DestroyBlock(localIndex);

	// 露出状態更新
	chunk->SetExposedAroundBlocks(localIndex);

	// ドロップアイテム生成
	AddDropItemAt(targetBlock->position_, BlockIdToDropItemId(destroyedId));
}

void MapManager::AddDropItemAt(const Vector3& position, ItemID id)
{
	Vector3 dropPos = position;
	dropPos.x += Game::Math::RandFloat(-0.3f, 0.3f, 2);
	dropPos.z += Game::Math::RandFloat(-0.3f, 0.3f, 2);
	dropItemManager_->AddItem(id, dropPos);
}

// 指定位置にブロックを設置
bool MapManager::SetBlockAt(const Vector2int& chunkPos, const Vector3int& localIndex, const BlockID id)
{
	// 設置するチャンクを取得
	Chunk* chunk = TryGetChunk(chunkPos);
	if (!chunk) return false;

	// 設置するブロック単位の空間を取得
	Block* targetBlock = chunk->blocks_[localIndex.x][localIndex.y][localIndex.z].get();
	if (!targetBlock) return false;

	// Air ブロックは設置できない
	if (id == BlockID::Air) return false;

	// 指定位置に既にブロックが存在しているなら設置できない
	if (targetBlock->GetBlockID() != BlockID::Air) return false;

	// キャラクターと重なってたら設置できない
	const AABB placeAabb = GetAABB(chunkPos, localIndex);
	if (IsOverlappingAnyCharacter(placeAabb))
	{
		return false;
	}

	// ブロック設置
	chunk->SetBlock(localIndex, id);

	// 露出状態更新
	chunk->SetExposedAroundBlocks(localIndex);

	return true;
}
bool MapManager::SetBlockAt(const lookAtBlock& lab, const BlockID id)
{
	// Air ブロックならreturn
	if (id == BlockID::Air) return false;
	// 向きが不明ならreturn
	if (lab.face == AABBFace::NONE) return false;

	Vector3int offset = Vector3int(0, 0, 0);

	switch (lab.face)
	{
	case AABBFace::LEFT:
		offset.x = -1;
		break;
	case AABBFace::RIGHT:
		offset.x = 1;
		break;
	case AABBFace::BOTTOM:
		offset.y = -1;
		break;
	case AABBFace::TOP:
		offset.y = 1;
		break;
	case AABBFace::BACK:
		offset.z = -1;
		break;
	case AABBFace::FRONT:
		offset.z = 1;
		break;
	default:
		return false;
		break;
	}

	Vector2int chunkPos = lab.chunkIndex;
	Vector3int localIndex = lab.localIndex + offset;

	// チャンク跨ぎ対応
	if (localIndex.x < 0)
	{
		chunkPos.x -= 1;
		localIndex.x += CHUNK_X;
	}
	else if (localIndex.x >= CHUNK_X)
	{
		chunkPos.x += 1;
		localIndex.x -= CHUNK_X;
	}
	if (localIndex.z < 0)
	{
		chunkPos.y -= 1;
		localIndex.z += CHUNK_Z;
	}
	else if (localIndex.z >= CHUNK_Z)
	{
		chunkPos.y += 1;
		localIndex.z -= CHUNK_Z;
	}

	return SetBlockAt(chunkPos, localIndex, id);
}
bool MapManager::SetBlockAt(const Vector3& position, const BlockID id)
{
	return SetBlockAt(ChunkIndexByPosition(position), BlockIndexByPosition(position), id);
}

// 初期
bool MapManager::SweepAABB(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const
{
	outCorrectedDelta = delta;

	// めり込み防止のスキン
	const float skin = 0.001f;

	// 与えられたAABBが重なり得るブロックを走査する
	auto AnySolidOverlap = [&](const AABB& test) -> bool
		{
			// AABBの覆うワールドブロック範囲（min/maxは「セルインデックス」）
			const Vector3int minBlockIndex = WorldBlockIndexByPosition(test.min);
			const Vector3int maxBlockIndex = WorldBlockIndexByPosition(test.max - Vector3{ 1e-6f, 1e-6f, 1e-6f });

			// floor_div（負数対応の床除算）
			auto FloorDiv = [](int a, int n) -> int
				{
					// n > 0 前提
					int q = a / n;
					int r = a % n;
					if (r != 0 && ((r > 0) != (n > 0)) != (a > 0)) {} // unused guard
					// C++の / は0方向丸めなので、負数で割り切れないときだけ1引く
					if (r != 0 && ((a < 0) ^ (n < 0))) --q;
					return q;
				};

			for (int by = minBlockIndex.y; by <= maxBlockIndex.y; ++by)
			{
				// Y軸はワールド境界でクリップ
				if (by < 0 || by >= CHUNK_Y) continue;
				const int localY = by;

				for (int bz = minBlockIndex.z; bz <= maxBlockIndex.z; ++bz)
				{
					const int chunkZ = FloorDiv(bz, CHUNK_Z);
					const int localZ = LocalMod(bz, CHUNK_Z);

					for (int bx = minBlockIndex.x; bx <= maxBlockIndex.x; ++bx)
					{
						const int chunkX = FloorDiv(bx, CHUNK_X);
						const int localX = LocalMod(bx, CHUNK_X);

						const Vector2int chunkPos{ chunkX, chunkZ };
						Chunk* c = TryGetChunk(chunkPos);
						if (!c) continue;

						Block* b = c->blocks_[localX][localY][localZ].get();
						if (!b) continue;
						if (b->GetBlockID() == BlockID::Air) continue;

						// ブロックAABBと実際に重なっているか
						if (IsOverLap(test, b->aabb_))
						{
							return true;
						}
					}
				}
			}
			return false;
		};

	// 「指定軸だけ」動かして、めり込むならその軸の移動量を詰める
	auto ResolveAxis = [&](float& dAxis, int axis) -> bool
		{
			if (std::abs(dAxis) < 1e-6f) return false;

			const float sign = (dAxis >= 0.0f) ? 1.0f : -1.0f;
			const float dist = std::abs(dAxis);

			bool hit = false;

			// 二分探索で「当たらない最大距離」を探す（0..dist）
			float lo = 0.0f;
			float hi = dist;

			auto MakeMove = [&](float signedAmount) -> Vector3
				{
					Vector3 move{ 0,0,0 };
					if (axis == 0) move.x = signedAmount;
					if (axis == 1) move.y = signedAmount;
					if (axis == 2) move.z = signedAmount;
					return move;
				};

			// dist動かしても当たらないなら終了
			{
				const Vector3 move = MakeMove(sign * dist);
				Vector3 testMove = outCorrectedDelta + move;
				const AABB test = aabb + testMove;
				if (!AnySolidOverlap(test)) return false;
			}

			// 当たるので探索
			for (int i = 0; i < 16; ++i)
			{
				const float mid = (lo + hi) * 0.5f;
				const Vector3 move = MakeMove(sign * mid);
				Vector3 testMove = outCorrectedDelta + move;
				const AABB test = aabb + testMove;

				if (AnySolidOverlap(test))
				{
					hi = mid;
					hit = true;
				}
				else
				{
					lo = mid;
				}
			}

			// skin分だけ手前で止める（距離側で引く）
			float allowedDist = lo - skin;
			if (allowedDist < 0.0f) allowedDist = 0.0f;

			dAxis = sign * allowedDist;
			return hit;
		};

	bool hitAny = false;

	// X
	{
		float dx = outCorrectedDelta.x;
		if (ResolveAxis(dx, 0)) hitAny = true;
		outCorrectedDelta.x = dx;
	}

	// Z
	{
		float dz = outCorrectedDelta.z;
		if (ResolveAxis(dz, 2)) hitAny = true;
		outCorrectedDelta.z = dz;
	}

	// Y
	{
		float dy = outCorrectedDelta.y;
		if (ResolveAxis(dy, 1)) hitAny = true;
		outCorrectedDelta.y = dy;
	}


	return hitAny;
}
// 1) depenetration（先に「微妙な重なり」を毎フレ解消してから軸解決）
bool MapManager::SweepAABB_DepentrationFirst(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const
{
	outCorrectedDelta = delta;
	const float skin = 0.001f;

	// 0) depenetration（今フレーム開始時点の微小めり込みを解消）
	//    ※ここでは「最小押し戻しベクトル」を厳密に求めず、接触してるブロックの深度を足し合わせて小さく押し戻す。
	{
		AABB cur = aabb;
		const int iters = 4;            // 少なすぎると残る / 多すぎると重い
		const float maxPush = 0.05f;    // 1フレで押し戻し過ぎない安全弁

		for (int iter = 0; iter < iters; ++iter)
		{
			Vector3 push(0, 0, 0);
			bool overlapped = false;

			ForEachSolidBlockAABB_OverlappingRange(this, cur, [&](const AABB& block)
				{
					if (!IsOverLap(cur, block)) return;

					overlapped = true;
					// AABB::GetCollisionDepth は「衝突している時は深度ベクトル」を返す前提
					// ※方向は実装依存なので、center比較で符号を付け直す
					Vector3 depth = cur.GetCollisionDepth(block);

					const Vector3 cC = cur.center();
					const Vector3 cB = block.center();

					// 最浅軸を選んでその軸だけ押す（順番依存を減らす）
					float pen = depth.x;
					int axis = 0;
					if (depth.y < pen) { pen = depth.y; axis = 1; }
					if (depth.z < pen) { pen = depth.z; axis = 2; }

					Vector3 localPush(0, 0, 0);
					if (axis == 0) localPush.x = (cC.x < cB.x) ? -pen : +pen;
					if (axis == 1) localPush.y = (cC.y < cB.y) ? -pen : +pen;
					if (axis == 2) localPush.z = (cC.z < cB.z) ? -pen : +pen;

					// skin分だけ余裕を取る
					localPush *= 1.0f + skin;

					push += localPush;
				});

			if (!overlapped) break;

			// 押し戻し過ぎない
			push.x = std::clamp(push.x, -maxPush, +maxPush);
			push.y = std::clamp(push.y, -maxPush, +maxPush);
			push.z = std::clamp(push.z, -maxPush, +maxPush);

			// 押し戻し
			cur = cur + push;

			// 実際の移動デルタ側にも反映（開始位置がズレた分だけ、deltaを補正する）
			outCorrectedDelta -= push;
		}
	}

	// 1) あなたの既存方式（軸ごとの二分探索）をそのまま呼ぶ形で書く
	auto ResolveAxis = [&](float& dAxis, int axis) -> bool
		{
			if (std::abs(dAxis) < 1e-6f) return false;

			const float sign = (dAxis >= 0.0f) ? 1.0f : -1.0f;
			const float dist = std::abs(dAxis);

			float lo = 0.0f;
			float hi = dist;

			auto MakeMove = [&](float signedAmount) -> Vector3
				{
					Vector3 move{ 0,0,0 };
					if (axis == 0) move.x = signedAmount;
					if (axis == 1) move.y = signedAmount;
					if (axis == 2) move.z = signedAmount;
					return move;
				};

			// dist動かしても当たらないならOK
			{
				const AABB test = aabb + (outCorrectedDelta + MakeMove(sign * dist));
				if (!AnySolidOverlap_Map(this, test)) return false;
			}

			// 当たるので探索
			for (int i = 0; i < 16; ++i)
			{
				const float mid = (lo + hi) * 0.5f;
				const AABB test = aabb + (outCorrectedDelta + MakeMove(sign * mid));
				if (AnySolidOverlap_Map(this, test)) hi = mid;
				else lo = mid;
			}

			float allowedDist = lo - skin;
			if (allowedDist < 0.0f) allowedDist = 0.0f;

			dAxis = sign * allowedDist;
			return true;
		};

	bool hitAny = false;

	// 推奨順：X→Z→Y（あなたの「直った順」）
	{
		float dx = outCorrectedDelta.x;
		if (ResolveAxis(dx, 0)) hitAny = true;
		outCorrectedDelta.x = dx;
	}
	{
		float dz = outCorrectedDelta.z;
		if (ResolveAxis(dz, 2)) hitAny = true;
		outCorrectedDelta.z = dz;
	}
	{
		float dy = outCorrectedDelta.y;
		if (ResolveAxis(dy, 1)) hitAny = true;
		outCorrectedDelta.y = dy;
	}

	return hitAny;
}
// 2) 真正のSweep（TOI: time of impact）寄せ（「一番早く当たる面」を探して進める）
bool MapManager::SweepAABB_TOI(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const
{
	outCorrectedDelta = delta;

	const float skin = 0.001f;

	// 開始時点でめり込んでるなら、TOIは破綻するので軽く押し戻す（最小限）
	if (AnySolidOverlap_Map(this, aabb))
	{
		outCorrectedDelta = Vector3{ 0,0,0 };
		return true;
	}

	// 探索対象は「移動AABBを含む範囲」＝ swept broadphase AABB
	AABB broad = aabb;
	broad = broad + delta; // 端点含む（AABB::operator+ がmin/maxを両方動かす前提）
	// ※本当は min/maxを吸収する broadphase を作るべきだが、ここでは簡易に「終点AABBも見る」方向

	SweepHit best;
	best.t = 1.0f;

	ForEachSolidBlockAABB_OverlappingRange(this, broad, [&](const AABB& block)
		{
			SweepHit h = SweptAABBvsAABB(aabb, delta, block);
			if (!h.hit) return;
			if (h.t < best.t)
			{
				best = h;
			}
		});

	if (!best.hit) return false;

	// 当たる直前まで移動（skin分引く）
	const float tMove = my_max(0.0f, best.t - (skin / (delta.Length() + 1e-6f)));
	outCorrectedDelta = delta * tMove;
	return true;
}
// 3) がっつりMTV方式（重なっているブロックとの“最小押し戻しベクトル”を計算して解消）
bool MapManager::SweepAABB_MTV(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const
{
	outCorrectedDelta = delta;
	const float skin = 0.001f;

	// まず通常の移動を仮適用して、終点でめり込むならMTVで押し戻す方式にする
	AABB moved = aabb + delta;

	// ① 終点がめり込んでないならそのままOK
	if (!AnySolidOverlap_Map(this, moved))
	{
		return false;
	}

	// ② めり込むので「終点での押し戻し」で corrected delta を作る（押し戻した分だけdeltaを短く）
	Vector3 totalPush(0, 0, 0);

	const int iters = 8;
	for (int iter = 0; iter < iters; ++iter)
	{
		bool overlapped = false;

		Vector3 bestPush(0, 0, 0);
		float bestMag = std::numeric_limits<float>::infinity();

		ForEachSolidBlockAABB_OverlappingRange(this, moved, [&](const AABB& block)
			{
				if (!IsOverLap(moved, block)) return;
				overlapped = true;

				const Vector3 depth = moved.GetCollisionDepth(block);

				// 最浅軸を選ぶ
				float pen = depth.x;
				int axis = 0;
				if (depth.y < pen) { pen = depth.y; axis = 1; }
				if (depth.z < pen) { pen = depth.z; axis = 2; }

				const Vector3 cM = moved.center();
				const Vector3 cB = block.center();

				Vector3 push(0, 0, 0);
				if (axis == 0) push.x = (cM.x < cB.x) ? -(pen + skin) : +(pen + skin);
				if (axis == 1) push.y = (cM.y < cB.y) ? -(pen + skin) : +(pen + skin);
				if (axis == 2) push.z = (cM.z < cB.z) ? -(pen + skin) : +(pen + skin);

				const float mag = std::abs((axis == 0) ? push.x : (axis == 1) ? push.y : push.z);
				if (mag < bestMag)
				{
					bestMag = mag;
					bestPush = push;
				}
			});

		if (!overlapped) break;

		// 最小押し戻し1発を適用
		moved = moved + bestPush;
		totalPush += bestPush;
	}

	// corrected = delta + totalPush（押し戻し方向は“めり込み解消”なので、deltaから見ると逆方向に戻ることが多い）
	outCorrectedDelta = delta + totalPush;
	return true;
}
// 4) サンプル点方式（面上の 3x3 サンプル点で押し戻し方向を決める）
bool MapManager::SweepAABB_SamplePoints(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const
{
	outCorrectedDelta = delta;
	const float skin = 0.001f;

	// まずは「予定移動」を適用したAABBを作って、めり込みがあるなら押し戻し
	AABB moved = aabb + delta;

	if (!AnySolidOverlap_Map(this, moved)) return false;

	auto PointInsideAnySolid = [&](const Vector3& p) -> bool
		{
			// 点を含むブロックを1個見る（高速）
			// ※ブロック座標変換は MapManager に既にある
			const Vector3int wb = WorldBlockIndexByPosition(p);
			if (wb.y < 0 || wb.y >= CHUNK_Y) return false;

			const int cx = FloorDivInt(wb.x, CHUNK_X);
			const int cz = FloorDivInt(wb.z, CHUNK_Z);
			const int lx = LocalMod(wb.x, CHUNK_X);
			const int lz = LocalMod(wb.z, CHUNK_Z);

			Chunk* c = TryGetChunk(Vector2int{ cx, cz });
			if (!c) return false;

			Block* b = c->blocks_[lx][wb.y][lz].get();
			if (!b) return false;
			if (b->GetBlockID() == BlockID::Air) return false;

			// 点がブロックAABB内か
			const AABB& ba = b->aabb_;
			return (ba.min.x <= p.x && p.x <= ba.max.x &&
				ba.min.y <= p.y && p.y <= ba.max.y &&
				ba.min.z <= p.z && p.z <= ba.max.z);
		};

	// 押し戻す方向は「移動方向の逆」から始めて、サンプル点の侵入が減る方向を探す
	// ここでは簡易に「X/Zの押し戻しを優先して、最後にY」を試す
	Vector3 corrected = delta;

	auto TryPushAxis = [&](int axis, float sign) -> bool
		{
			// sign: +1 なら +軸へ押す
			const float step = 0.01f;      // 押し戻し刻み（粗いほど軽いが貫通しやすい）
			const int maxSteps = 20;

			for (int i = 0; i < maxSteps; ++i)
			{
				Vector3 push(0, 0, 0);
				if (axis == 0) push.x = sign * step;
				if (axis == 1) push.y = sign * step;
				if (axis == 2) push.z = sign * step;

				AABB test = moved + push;

				// 動いてる方向の面をサンプル（押し戻し方向と逆側の面を見るのがコツ）
				Vector3 pts[9];
				if (axis == 0)
				{
					const float faceX = (sign > 0) ? test.max.x : test.min.x;
					MakeFaceSamplePoints_X(test, faceX, pts);
				}
				else if (axis == 1)
				{
					const float faceY = (sign > 0) ? test.max.y : test.min.y;
					MakeFaceSamplePoints_Y(test, faceY, pts);
				}
				else
				{
					const float faceZ = (sign > 0) ? test.max.z : test.min.z;
					MakeFaceSamplePoints_Z(test, faceZ, pts);
				}

				int insideCount = 0;
				for (int k = 0; k < 9; ++k)
				{
					if (PointInsideAnySolid(pts[k])) insideCount++;
				}

				// サンプル点が全部外に出たら「その方向で押し戻せた」とみなす
				if (insideCount == 0 && !AnySolidOverlap_Map(this, test))
				{
					// skin分追加
					Vector3 extra(0, 0, 0);
					if (axis == 0) extra.x = sign * skin;
					if (axis == 1) extra.y = sign * skin;
					if (axis == 2) extra.z = sign * skin;

					moved = test + extra;
					corrected += (push + extra);
					return true;
				}

				// まだダメなら押し戻しを積み増し
				moved = test;
				corrected += push;
			}
			return false;
		};

	// 押し込んでるときに落下が止まりやすいので、X/Z優先→最後にY
	if (delta.x != 0.0f) TryPushAxis(0, (delta.x > 0) ? -1.0f : +1.0f);
	if (delta.z != 0.0f) TryPushAxis(2, (delta.z > 0) ? -1.0f : +1.0f);
	if (delta.y != 0.0f) TryPushAxis(1, (delta.y > 0) ? -1.0f : +1.0f);

	outCorrectedDelta = corrected;
	return true;
}

bool MapManager::isSolidAt(const Vector3& position) const
{
	Vector2int chunkPos = ChunkIndexByPosition(position);
	Vector3int index = BlockIndexByPosition(position);
	Chunk* chunk = TryGetChunk(chunkPos);
	if (chunk)
	{
		Block* block = chunk->blocks_[index.x][index.y][index.z].get();
		if (block && block->GetBlockID() != BlockID::Air)
		{
			return true;
		}
	}
	return false;
}

bool MapManager::IsOverlappingAnyCharacter(const AABB& aabb) const
{
	for (BaseCharacter* c : Characters_)
	{
		if (!c) continue;
		const AABB& ca = c->data_.aabbs[0];
		if (IsOverLap(aabb, ca))
			return true;
	}
	return false;
}

AABB MapManager::GetAABB(const Vector2int& chunkPos, const Vector3int& index) const
{
	// チャンクのワールド原点
	float chunkWorldX = chunkPos.x * CHUNK_X * BLOCK_SIZE;
	float chunkWorldZ = chunkPos.y * CHUNK_Z * BLOCK_SIZE;

	// ブロックのワールド座標
	float worldX = chunkWorldX + index.x * BLOCK_SIZE;
	float worldY = index.y * BLOCK_SIZE;
	float worldZ = chunkWorldZ + index.z * BLOCK_SIZE;

	Vector3 mint(worldX, worldY, worldZ);
	Vector3 maxt(worldX + BLOCK_SIZE, worldY + BLOCK_SIZE, worldZ + BLOCK_SIZE);

	return AABB(mint, maxt);
}
AABB MapManager::GetAABB(const Vector3& position) const
{
	Vector2int chunkPos = ChunkIndexByPosition(position);
	Vector3int index = BlockIndexByPosition(position);
	return GetAABB(chunkPos, index);
}
bool MapManager::GetIsActive(const Vector2int& chunkPos, const Vector3int& index) const
{
	Chunk* chunk = TryGetChunk(chunkPos);
	if (chunk)
	{
		if (chunk->blocks_[index.x][index.y][index.z]->blockInfo_.type != BlockID::Air)
		{
			return true;
		}
	}
	return false;
}
bool MapManager::GetIsActive(const Vector3& position) const
{
	Vector2int chunkPos = ChunkIndexByPosition(position);
	Vector3int index = BlockIndexByPosition(position);
	return GetIsActive(chunkPos, index);
}

// position が今どのチャンクに属しているか  例：position=(34, 0, 50) -> chunkIndex=(1, 2)
Vector2int MapManager::ChunkIndexByPosition(const Vector3& position) const
{
	int bx = static_cast<int>(std::floor(position.x / BLOCK_SIZE));
	int bz = static_cast<int>(std::floor(position.z / BLOCK_SIZE));

	Vector2int chunk;
	chunk.x = static_cast<int>(std::floor((float)bx / CHUNK_X));
	chunk.y = static_cast<int>(std::floor((float)bz / CHUNK_Z));
	return chunk;
}

// position が今チャンク内どのブロックに属しているか(どんな時も0～CHUNK_SIZE-1の範囲に収まる)  例：position=(34, 0, 50) -> localIndex=(2, 0, 2)
Vector3int MapManager::BlockIndexByPosition(const Vector3& position) const
{
	// ① ワールド座標 → 世界ブロック座標
	Vector3int wb = WorldBlockIndexByPosition(position);

	// ② チャンク内インデックスへ正規化（数学的 mod）
	Vector3int local;
	local.x = LocalMod(wb.x, CHUNK_X);
	local.y = LocalMod(wb.y, CHUNK_Y);
	local.z = LocalMod(wb.z, CHUNK_Z);

	local.y = std::clamp(local.y, 0, CHUNK_Y - 1);

	return local;
}

Vector3int MapManager::WorldBlockIndexByPosition(const Vector3& position) const
{
	int bx = static_cast<int>(std::floor(position.x / BLOCK_SIZE));
	int by = static_cast<int>(std::floor(position.y / BLOCK_SIZE));
	int bz = static_cast<int>(std::floor(position.z / BLOCK_SIZE));
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
				int(std::floor(p.x / BLOCK_SIZE)),
				int(std::floor(p.y / BLOCK_SIZE)),
				int(std::floor(p.z / BLOCK_SIZE))
			);
		};

	auto WorldChunkIndexFromWorldBlock = [](const Vector3int& wb) -> Vector2int
		{
			Vector2int cp;
			cp.x = int(std::floor(float(wb.x) / float(CHUNK_X)));
			cp.y = int(std::floor(float(wb.z) / float(CHUNK_Z)));
			return cp;
		};

	auto Sign = [](float v) -> int { return (v > 0.0f) - (v < 0.0f); };

	auto LocalMod = [](int a, int n) -> int { return (a % n + n) % n; };

	auto LocalIndexFromWorldBlock = [&](const Vector3int& wb) -> Vector3int
		{
			return Vector3int(
				LocalMod(wb.x, CHUNK_X),
				LocalMod(wb.y, CHUNK_Y),
				LocalMod(wb.z, CHUNK_Z)
			);
		};

	// sideDist（次の境界までの距離 t）
	auto NextBoundaryT = [](float origin, float dir, int cell, int step) -> float
		{
			// cell = floor(origin/BLOCK_SIZE) のブロック座標
			if (step > 0)
			{
				const float next = (float(cell) + 1.0f) * BLOCK_SIZE;
				return (next - origin) / dir;
			}
			else
			{
				const float next = float(cell) * BLOCK_SIZE;
				return (next - origin) / dir;
			}
		};

	// 現在のワールドブロックインデックス
	Vector3int currentWB = WorldBlockIndexByPosition(rayStart);
	const Vector3int endWB = WorldBlockIndexByPosition(rayEnd);

	// step（dir==0 の軸は 0）
	const int stepX = Sign(dir.x);
	const int stepY = Sign(dir.y);
	const int stepZ = Sign(dir.z);

	// deltaDist（dir==0 は INF 扱い）
	const float INF = std::numeric_limits<float>::infinity();
	const float deltaDistX = (std::abs(dir.x) < 1e-6f) ? INF : std::abs(BLOCK_SIZE / dir.x);
	const float deltaDistY = (std::abs(dir.y) < 1e-6f) ? INF : std::abs(BLOCK_SIZE / dir.y);
	const float deltaDistZ = (std::abs(dir.z) < 1e-6f) ? INF : std::abs(BLOCK_SIZE / dir.z);

	float sideDistX = (stepX == 0) ? INF : NextBoundaryT(rayStart.x, dir.x, currentWB.x, stepX);
	float sideDistY = (stepY == 0) ? INF : NextBoundaryT(rayStart.y, dir.y, currentWB.y, stepY);
	float sideDistZ = (stepZ == 0) ? INF : NextBoundaryT(rayStart.z, dir.z, currentWB.z, stepZ);

	// 最大ステップ
	const int maxSteps = 2048;

	// 直前に跨いだ面（＝ currentWB に入った面）
	AABBFace enterFace = AABBFace::NONE;


	for (int i = 0; i < maxSteps; ++i)
	{
		// currentWB -> chunk/local
		const Vector2int chunkPos = WorldChunkIndexFromWorldBlock(currentWB);
		const Vector3int local = LocalIndexFromWorldBlock(currentWB);

		Chunk* chunk = TryGetChunk(chunkPos);
		if (chunk)
		{
			// local.y は縦制限があるので範囲チェック
			if (0 <= local.x && local.x < CHUNK_X &&
				0 <= local.y && local.y < CHUNK_Y &&
				0 <= local.z && local.z < CHUNK_Z)
			{
				Block* b = chunk->blocks_[local.x][local.y][local.z].get();
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
					case AABBFace::LEFT:   dist = std::abs(aabb.min.x - rayStart.x); break;
					case AABBFace::RIGHT:  dist = std::abs(aabb.max.x - rayStart.x); break;
					case AABBFace::BOTTOM: dist = std::abs(aabb.min.y - rayStart.y); break;
					case AABBFace::TOP:    dist = std::abs(aabb.max.y - rayStart.y); break;
					case AABBFace::BACK:   dist = std::abs(aabb.min.z - rayStart.z); break;
					case AABBFace::FRONT:  dist = std::abs(aabb.max.z - rayStart.z); break;
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
				enterFace = (stepX > 0) ? AABBFace::LEFT : AABBFace::RIGHT;

				sideDistX += deltaDistX;
			}
			else
			{
				// Z方向へ進む
				currentWB.z += stepZ;

				// stepZ=+1 なら “BACK面から入る”、stepZ=-1 なら “FRONT面から入る”
				enterFace = (stepZ > 0) ? AABBFace::BACK : AABBFace::FRONT;

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
				enterFace = (stepY > 0) ? AABBFace::BOTTOM : AABBFace::TOP;

				sideDistY += deltaDistY;
			}
			else
			{
				// Z方向へ進む
				currentWB.z += stepZ;

				enterFace = (stepZ > 0) ? AABBFace::BACK : AABBFace::FRONT;

				sideDistZ += deltaDistZ;
			}
		}

		// 伸びすぎ防止：レイ長を超えたら終了（tの近似として最小sideDistを使う）
		const float tApprox = my_min(sideDistX, my_min(sideDistY, sideDistZ));
		if (tApprox > rayLen) return std::nullopt;
	}

	return std::nullopt;
}

RayHitResult MapManager::GetFirstHitByRay(const Ray& ray, float maxDistance, const BaseCharacter* ignore) const
{
	RayHitResult best{};

	// 1) ブロック
	if (auto b = GetBlockByCrossedRay(ray, maxDistance); b.has_value())
	{
		best.type = RayHitResult::Type::Block;
		best.blockHit = b.value();
		best.distance = b->distance;
	}

	// 2) キャラ（登録済みキャラのAABBと判定）
	float bestCharDist = std::numeric_limits<float>::infinity();
	BaseCharacter* bestChar = nullptr;

	// ※ Characters_ を保持している前提（前の実装で追加）
	for (BaseCharacter* c : Characters_)
	{
		if (!c) continue;
		if (c == ignore) continue;

		// 自分自身を除外したい場合は呼び出し側で ray.origin の所有者を渡す仕組みが必要。
		// ここでは「Rayの原点がそのキャラAABB内なら無視」程度で回避する。
		const AABB& ca = c->data_.aabbs[0];

		float t = 0.0f;
		if (!RayIntersectAABB_FirstT(ray, ca, t)) continue;
		if (t < 0.0f) continue;

		const float dist = t; // ray.diff が正規化されている前提。Player/Enemyでは Normalized() している。
		if (dist <= maxDistance && dist < bestCharDist)
		{
			bestCharDist = dist;
			bestChar = c;
		}
	}

	if (bestChar)
	{
		// ブロックより手前ならキャラ優先
		if (best.type == RayHitResult::Type::None || bestCharDist < best.distance)
		{
			best.type = RayHitResult::Type::Character;
			best.Character = bestChar;
			best.distance = bestCharDist;
		}
	}

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

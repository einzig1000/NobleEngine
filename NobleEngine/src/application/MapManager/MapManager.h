#pragma once
#include <string>
#include "Game.h"
#include <queue>
#include <unordered_set>
#include <memory>
#include "Utilities/PerlinNoise.h"
#include "engine/Physics/IWorldCollider.h"

class Block;
class Chunk;
class Player;
class BlockConfig;
class DropItemManager;
class BaseCharacter;



class MapManager// : public IWorldCollider
{
public:
	MapManager();
	void SetPlayer(Player* player);
	~MapManager();

	void LoadNameAndPathMap(const std::string& filePath);
	void SaveNameAndPathMap(const std::string& filePath);
	void LoadMap(const std::string& mapName);
	void SaveMap();
	void CreateNewMap(const std::string& mapName, uint32_t seed);

	void Initialize();
	void Update();
	void Draw();
	void DrawImGui();

	void SetDrawRadius(int r) { drawRadius_ = r; }
	void SetUpdateRadius(int r) { updateRadius_ = r; }

	// ブロック破壊
	void DestroyBlockAt(const Vector2int& chunkPos, const Vector3int& localIndex);
	void AddDropItemAt(const Vector3& position, ItemID id);
	// ブロック設置
	bool SetBlockAt(const lookAtBlock& lab, const BlockID id);
	bool SetBlockAt(const Vector2int& chunkPos, const Vector3int& localIndex, const BlockID id);
	bool SetBlockAt(const Vector3& position, const BlockID id);

	// 0) 軸解決のみ
	bool SweepAABB(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const;
	// 1) defenestration（先に「微妙な重なり」を毎フレ解消してから軸解決）
	bool SweepAABB_DepentrationFirst(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const;
	// 2) 真正のSweep（TOI: time of impact）寄せ（「一番早く当たる面」を探して進める）
	bool SweepAABB_TOI(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const;
	// 3) がっつりMTV方式（重なっているブロックとの“最小押し戻しベクトル”を計算して解消）
	bool SweepAABB_MTV(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const;
	// 4) サンプル点方式（面上の 3x3 サンプル点で押し戻し方向を決める）
	bool SweepAABB_SamplePoints(const AABB& aabb, const Vector3& delta, Vector3& outCorrectedDelta) const;

	// 指定位置に固体ブロックがあるか
	bool isSolidAt(const Vector3& position) const;
	// 指定AABBにキャラがあるか
	bool IsOverlappingAnyCharacter(const AABB& aabb) const;

	// レイとブロックの交差判定（衝突ブロックを返す）
	std::optional<lookAtBlock> GetBlockByCrossedRay(const Ray& ray, const float maxDistance) const;
	// ブロック/キャラのうち最初に当たったものを返す
	RayHitResult GetFirstHitByRay(const Ray& ray, float maxDistance, const BaseCharacter* ignore) const;
	// レイとブロックの交差判定（衝突座標を返す）
	std::optional<Vector3> GetPositionByCrossedRay(const Ray& ray) const;

	AABB GetAABB(const Vector2int& chunkPos, const Vector3int& index) const;
	AABB GetAABB(const Vector3& position) const;
	bool GetIsActive(const Vector2int& chunkPos, const Vector3int& index) const;
	bool GetIsActive(const Vector3& position) const;
	Vector2int ChunkIndexByPosition(const Vector3& position) const;		// ワールド座標 → chunksのキーインデックス座標
	Vector3int BlockIndexByPosition(const Vector3& position) const;		// ワールド座標 → ブロックインデックス座標
	// ワールド座標からワールドブロックインデックス
	Vector3int WorldBlockIndexByPosition(const Vector3& position) const;

	// マップデータ
	std::unordered_map<Vector2int, std::unique_ptr<Chunk>, Vector2intHash> chunks;
	
	// チャンク有無確認
	bool HasChunk(const Vector2int& chunkPos) const;
	// チャンク取得、なくても生成はしない
	Chunk* TryGetChunk(const Vector2int& chunkPos) const;
	// チャンク取得、なければスケジュールに登録して生成
	Chunk* GetOrCreateChunk(const Vector2int& chunkPos);
	// 欲しいチャンクが存在しなければスケジュールに登録
	void EnsureChunkScheduled(const Vector2int& chunkPos);
	// スケジュールに登録されたチャンクを1Fに1つ生成
	void ProcessChunkGeneration();

	// キャラクター登録
	void RegisterCharacter(BaseCharacter* c) { Characters_.push_back(c); }
	void UnregisterCharacter(BaseCharacter* c) 
	{ 
		Characters_.erase(std::remove(Characters_.begin(), Characters_.end(), c), Characters_.end()); 
	}

	void SetSeed(uint32_t seed); 
	uint32_t GetSeed() const { return noiseParam_.seed; }

	// マップネーム->ファイルパスマップ
	std::map<std::string, std::string> mapNameToFilePath_;

private:
	// キャラクター管理
	std::vector<BaseCharacter*> Characters_;

	// マップファイルパス
	std::string currentMapFilePath_;
	// マップネーム
	std::string currentMapName_;


	// プレイヤー参照
	Player* player_;

	// ドロップアイテム管理
	std::unique_ptr<DropItemManager> dropItemManager_;

	// このキューに入っているチャンクを順次生成していく
	std::queue<Vector2int> chunkGenQueue_;		
	// スケジュール済みチャンク集合
	std::unordered_set<Vector2int, Vector2intHash> chunkScheduled_;
	// 既に作成されたチャンク集合
	std::unordered_set<Vector2int, Vector2intHash> chunkCreated_;

	// パラメータ
	int drawRadius_ = 10;    // 描画半径（チャンク単位）
	int updateRadius_ = 2;   // 更新半径（チャンク単位）
	NoiseParameter noiseParam_;

};


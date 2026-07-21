#pragma once
#include <string>
#include <Game.h>
#include <queue>
#include <unordered_set>
#include <memory>
#include <PerlinNoise.h>
#include <definition/definition.h>
#include <DrawSystem/RenderData/RenderObject.h>

class Block;
class Chunk;
class Player;
class BlockConfig;
class DropItemManager;
class BaseCharacter;

class MapManager
{
public:
	MapManager();
	~MapManager();

	void LoadNameAndPathMap(const std::string& filePath);
	void SaveNameAndPathMap(const std::string& filePath);
	void LoadMap(const std::string& mapName);
	void SaveMap();
	void CreateNewMap(const std::string& mapName, uint32_t seed);

	void Initialize();
	void Update(int32_t cameraID);
	void Draw(int32_t renderTargetID);
	void DrawImGui();

	void SetDrawRadius(const Vector3int& r) { drawRadius_ = r; }
	void SetUpdateRadius(const Vector3int& r) { updateRadius_ = r; }

	// ブロック破壊
	void DestroyBlockAt(const Vector3int& chunkPos, const Vector3int& localIndex);
	void AddDropItemAt(const Vector3& position, ItemID id);

	// AABB内のブロックを破壊する
	void DestroyBlockInAABB(const AABB& aabb);

	// ブロック設置
	bool SetBlockAt(const lookAtBlock& lab, BlockID id);
	bool SetBlockAt(const Vector3int& chunkPos, const Vector3int& localIndex, BlockID id);
	bool SetBlockAt(const Vector3& position, BlockID id);

	
	/// <summary>
	/// 強くなった自分で衝突判定再設計
	/// </summary>
	/// <param name="aabb">キャラクター衝突判定AABB</param>
	/// <param name="movement">申請移動量(衝突していた場合修正される)</param>
	void SweepAABB(const AABB& aabb, Vector3& movement);

	/// <summary>
	/// 指定ブロック分進めるか。
	/// </summary>
	/// <param name="aabb">キャラクター衝突判定AABB</param>
	/// <param name="face">衝突判定したい面</param>
	/// <param name="layerCount">何層分進むか</param>
	/// <returns>何層分進めたか</returns>
	int32_t SweepAABB(const AABB& aabb, AABBFace face, int32_t layerCount);


	// 指定AABBにキャラがあるか
	bool IsOverlappingAnyCharacter(const AABB& aabb) const;

	// 指定位置に固体ブロックがあるか
	bool IsSolidAt(const Vector3& position) const;

	// レイとブロックの交差判定（衝突ブロックを返す）
	std::optional<lookAtBlock> GetBlockByCrossedRay(const Ray& ray, const float maxDistance) const;
	// ブロック/キャラのうち最初に当たったものを返す
	RayHitResult GetFirstHitByRay(const Ray& ray, float maxDistance, const BaseCharacter* ignore) const;
	// レイとブロックの交差判定（衝突座標を返す）
	std::optional<Vector3> GetPositionByCrossedRay(const Ray& ray) const;

	AABB GetAABB(const Vector3int& chunkPos, const Vector3int& index) const;
	AABB GetAABB(const Vector3& position) const;
	bool GetIsActive(const Vector3int& chunkPos, const Vector3int& index) const;
	bool GetIsActive(const Vector3& position) const;
	Vector3int ChunkIndexByPosition(const Vector3& position) const;		// ワールド座標 → chunksのキーインデックス座標
	Vector3int BlockIndexByPosition(const Vector3& position) const;		// ワールド座標 → ブロックインデックス座標
	// ワールド座標からワールドブロックインデックス
	Vector3int WorldBlockIndexByPosition(const Vector3& position) const;


	// チャンク取得
	Chunk* GetChunk(const Vector3int& chunkPos) const;
	// 生成スケジュールに登録
	void EnsureChunkScheduled(const Vector3int& chunkPos);
	// スケジュールに登録されたチャンクを1つ生成
	void ProcessChunkGeneration(const Vector3int& cameraChunkPos);
	// 実際に生成
	void GenerateChunk(const Vector3int& chunkPos);

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
	// マップデータ
	std::unordered_map<Vector3int, std::unique_ptr<Chunk>, Vector3intHash> chunks;
	// スケジュール中チャンク集合
	std::unordered_set<Vector3int, Vector3intHash> chunkScheduled_;
	// 既に作成されたチャンク集合
	std::unordered_set<Vector3int, Vector3intHash> chunkCreated_;

	// キャラクター管理
	std::vector<BaseCharacter*> Characters_;

	// マップファイルパス
	std::string currentMapFilePath_;
	// マップネーム
	std::string currentMapName_;


	// パラメータ
	Vector3int drawRadius_;    // 描画半径（チャンク単位）
	Vector3int updateRadius_;   // 更新半径（チャンク単位）
	NoiseParameter noiseParam_;

	// カメラ座標
	Vector3int cameraChunkPos_;
};


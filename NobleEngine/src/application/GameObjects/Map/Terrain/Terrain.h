#pragma once
#include <string>
#include <Game.h>
#include <unordered_set>
#include <memory>
#include <Utilities/PerlinNoise.h>
#include <definition/definition.h>
#include <GameObjects/Map/Terrain/FaceDataPagePool/FaceDataPagePool.h>

class Chunk;
class ICharacter;
class EventBus;

class Terrain
{
public:
	Terrain();
	~Terrain();

	void Initialize();
	void Update(int32_t cameraID, Vector3 centerPos);
	void Draw(int32_t renderTargetID);
	void DrawImGui();

	void SetEventBus(EventBus* eventBus) { eventBus_ = eventBus; }

	void LoadNameAndPathMap(const std::string& filePath);
	void SaveNameAndPathMap(const std::string& filePath);
	void LoadMap(const std::string& mapName);
	void SaveMap();
	void CreateNewMap(const std::string& mapName, uint32_t seed);

	// チャンク一括生成
	void GenerateChunks(Vector3 pos);

	// チャンクの更新/描画範囲を設定する
	void SetDrawRadius(const Vector3int& r) { drawRadius_ = r; }
	void SetUpdateRadius(const Vector3int& r) { updateRadius_ = r; }

	// マップのシード値を設定する
	void SetSeed(uint32_t seed);

	// 指定範囲内のブロックを一括で置き換える
	void ReplaceBlockInAABB(const AABB& aabb, BlockID id);
	void ReplaceBlockInOBB(const OBB& obb, BlockID id);
	void ReplaceBlockInSphere(const Sphere& sphere, BlockID id);

	// 指定位置のブロックを置き換える 
	bool ReplaceBlock(const lookAtBlock& lab, BlockID id);
	bool ReplaceBlock(const Vector3int& chunkPos, const Vector3int& localIndex, BlockID id);
	bool ReplaceBlock(const Vector3& position, BlockID id);

	// 指定座標ブロックのワールドAABB/ワールドSphereを取得
	AABB GetAABB(const Vector3int& chunkPos, const Vector3int& index) const;
	AABB GetAABB(const Vector3& position) const;
	Sphere GetSphere(const Vector3int& chunkPos, const Vector3int& index) const;

	// ワールド座標からチャンクの整数座標/ブロックのチャンク内整数座標を取得
	Vector3int ChunkIndexByPosition(const Vector3& position) const;
	Vector3int BlockIndexByPosition(const Vector3& position) const;


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
	int32_t SweepAABB(const AABB& aabb, AABBFace face, int32_t layerCount) const;


	// 指定AABBにキャラがあるか
	bool IsOverlappingAnyCharacter(const AABB& aabb) const;

	// レイとブロックの交差判定（衝突ブロックを返す）
	std::optional<lookAtBlock> GetBlockByCrossedRay(const Ray& ray, const float maxDistance) const;
	// ブロック/キャラのうち最初に当たったものを返す
	RayHitResult GetFirstHitByRay(const Ray& ray, float maxDistance, const ICharacter* ignore) const;
	// レイとブロックの交差判定（衝突座標を返す）
	std::optional<Vector3> GetPositionByCrossedRay(const Ray& ray) const;


	// チャンク取得
	Chunk* GetChunk(const Vector3int& chunkPos) const;
	// 欲しいチャンクが存在しなければスケジュールに登録
	void EnsureChunkScheduled(const Vector3int& chunkPos);
	// スケジュールに登録されたチャンクを1つ生成
	void ProcessChunkGeneration(const Vector3int& cameraChunkPos);

	// キャラクター登録/解除
	void RegisterCharacter(ICharacter* c);
	void UnregisterCharacter(ICharacter* c);

	// マップネーム->ファイルパスマップ
	std::map<std::string, std::string> mapNameToFilePath_;

private:
	EventBus* eventBus_ = nullptr;

	// マップデータ
	std::unordered_map<Vector3int, std::unique_ptr<Chunk>, Vector3intHash> chunks;
	// スケジュール中チャンク集合
	std::unordered_set<Vector3int, Vector3intHash> chunkScheduled_;
	// 既に作成されたチャンク集合
	std::unordered_set<Vector3int, Vector3intHash> chunkCreated_;

	// 露出面プール
	std::unique_ptr<FaceDataPagePool> facePagePool_;

	// キャラクター管理
	std::vector<ICharacter*> Characters_;

	// マップファイルパス
	std::string currentMapFilePath_;
	// マップネーム
	std::string currentMapName_;

	// リアルタイム描画数
	int32_t drawCount_ = 0;

	// パラメータ
	Vector3int drawRadius_;    // 描画半径（チャンク単位）
	Vector3int updateRadius_;   // 更新半径（チャンク単位）
	NoiseParameter noiseParam_;

	// カメラ座標
	Vector3int cameraChunkPos_;
};



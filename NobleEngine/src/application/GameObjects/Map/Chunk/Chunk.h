#pragma once
#include <Game.h>
#include <Utilities/PerlinNoise.h>
#include <definition/definition.h>

class FaceDataPagePool;

struct ChunkInfo
{
	// このチャンクの左下奥ワールド座標原点(x-y-z-)
	Vector3 chunkWorldOrigin;
	// ブロックサイズ (Constexprs::kBlockSize)
	float blockSize = Constexprs::kBlockSize;
	// 各軸のブロック数 (Constexprs::kChunkBlockCount)
	Vector3uint blockCountXYZ = Vector3uint(Constexprs::kChunkBlockCountX, Constexprs::kChunkBlockCountY, Constexprs::kChunkBlockCountZ);
	// パディング
	uint32_t _pad0;
};

// 露出面ベイクの進行状況
enum class BakeState
{
	Idle,           // 何もしていない
	WaitingForCount // Phase1発行済み、総面数の読み戻し待ち
};

class Chunk
{
public:
	Chunk(const NoiseParameter& param, const Vector3int& chunkIndex, FaceDataPagePool* pagePool);
	~Chunk();
	void Update(int32_t cameraID);
	void Draw(int32_t renderTargetID);

	// 隣接チャンク管理
	void SetNeighborChunk(DirectionXYZ direction, Chunk* neighbor);
	bool IsNeighborExist(DirectionXYZ direction);

	// 指定座標のブロックを取得する
	BlockID* GetBlockID(const Vector3int& index);
	BlockID* GetBlockIDHelloNeighbor(const Vector3int& index);

	// ブロック設置(置換)
	void SetBlock(const Vector3int& localIndex, const BlockID id);
	void SetBlockIDs(const Vector3int& localIndex, BlockID id);
	void PushToNeighborHalo(DirectionXYZ direction, const Vector3int& localIndex, const BlockID id);

private:

	// 地形生成
	void CreateChunkData(const NoiseParameter& param);
	void CreateChunkDataFromJson();		// Jsonからチャンクデータを読み込み
	void CreateChunkDataNewly(const NoiseParameter& param);	// 新規生成チャンクデータ作成
	void GenerateOres(const NoiseParameter& param);	// 鉱石を生成
	void GenerateTrees(const NoiseParameter& param);// 木を生成


	// Phase1(カウントのみ)を発行し、読み戻しを要求する
	void DispatchCountPhase();
	// 読み戻し結果を受けて、ページ確保とPhase2(書き込み)を行う
	void FinishBakeWithPageAssignment(uint32_t totalFaces);

private:
	// 隣接チャンク static_cast<size_t>(DirectionXYZ)でアクセス
	Chunk* neighbors_[6];

	// チャンク情報
	Vector3int chunkIndex_;
	ChunkInfo chunkInfo_;
	AABB chunkAABB_;
	// ブロックデータ配列
	BlockID blocks_[Constexprs::kChunkBlockCountX][Constexprs::kChunkBlockCountY][Constexprs::kChunkBlockCountZ];
	uint32_t blockIds_[Constexprs::kMaxFacesPerChunkPlusHalo];


	// 描画オブジェクト
	std::unique_ptr<RenderObject> render_;
	// Phase1(カウントのみ)用の計算オブジェクト
	std::unique_ptr<ComputeObject> countCompute_;
	// Phase2(共有プールへの書き込み)用の計算オブジェクト
	std::unique_ptr<ComputeObject> writeCompute_;

	// ブロックID配列更新フラグ
	bool blockIdsDirty_ = false;
	void UpdateBlockIds();

	// ベイクの進行状況
	BakeState bakeState_ = BakeState::Idle;
	// 進行中の読み戻しトークン
	int32_t pendingReadbackToken_ = -1;

	// 全チャンク共有のページプール(所有はMapManager)
	FaceDataPagePool* pagePool_ = nullptr;
	// このチャンクが現在保有しているページ番号一覧
	std::vector<uint32_t> myPages_;



	// このチャンクのブロックID配列のリソースID
	int32_t blockIDsResourceID_ = -1;
	// 焼き込み済み面データのリソースID
	int32_t bakedFacesResourceID_ = -1;
	// グループごとの面数のリソースID
	int32_t faceCountResourceID_ = -1;
	// グループごとの書き込み開始オフセットのリソースID
	int32_t groupOffsetResourceID_ = -1;
	// 面カウンタ(InterlockedAdd用、1要素だけ)のリソースID
	int32_t faceWriteCounterResourceID_ = -1;

	bool inCamera_ = false;
};


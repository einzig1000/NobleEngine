#pragma once
#include <Game.h>
#include <Utilities/PerlinNoise.h>
#include <definition/definition.h>

class FaceDataPagePool;

struct ChunkInfo
{
	// このチャンクの左下奥ワールド座標原点(x-,y-,z-)
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
	WaitingForCount // Count.CSの総面数ReadBack待ち
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


	// Count.CSの起動・ReadBack申請
	void DispatchCountPhase();
	// 総面数ReadBack取得・ページ確保・Write.CSの起動
	void FinishBakeWithPageAssignment(uint32_t totalFaces);

private:
	// 隣接チャンク static_cast<size_t>(DirectionXYZ)でアクセス
	Chunk* neighbors_[6];
	// 全チャンク共有の面ページプール
	FaceDataPagePool* pagePool_ = nullptr;


	// チャンク情報
	Vector3int chunkIndex_;
	ChunkInfo chunkInfo_;
	AABB chunkAABB_;
	// ブロックデータ配列
	BlockID blocks_[Constexprs::kChunkBlockCountX][Constexprs::kChunkBlockCountY][Constexprs::kChunkBlockCountZ];
	uint32_t blockIds_[Constexprs::kMaxFacesPerChunkPlusHalo];

	// ブロックID配列更新フラグ
	bool blockIdsDirty_ = false;


	// 描画オブジェクト
	std::unique_ptr<RenderObject> render_;
	// Count.CS用の計算オブジェクト
	std::unique_ptr<ComputeObject> countCompute_;
	// Write.CS用の計算オブジェクト
	std::unique_ptr<ComputeObject> writeCompute_;


	// ベイクの進行状況
	BakeState bakeState_ = BakeState::Idle;
	// 進行中の読み戻しトークン
	int32_t pendingReadbackToken_ = -1;

	// 描画に使っているスロット番号(0 or 1)。Bakeは常にもう片方のスロットに対して行う
	int32_t activeSlot_ = 0;
	// このチャンクが現在保有しているページ番号一覧
	std::vector<uint32_t> myPages_[2];


	// このチャンクのブロックID配列のリソースID
	int32_t blockIDsResourceID_ = -1;
	// グループごとの面数のリソースID
	int32_t faceCountResourceID_[Constexprs::kFrameCount] = { -1, -1 };
	// グループごとの書き込み開始オフセットのリソースID
	int32_t groupOffsetResourceID_[Constexprs::kFrameCount] = { -1, -1 };
	// 面カウンタ(InterlockedAdd用、1要素だけ)のリソースID
	int32_t faceWriteCounterResourceID_ = -1;
	// 保有ページ番号一覧のリソースID(可変長)
	int32_t myPagesResourceID_[Constexprs::kFrameCount] = { -1, -1 };

	bool inCamera_ = false;


	// activeSlot_切り替え待ちのスロット番号(-1なら待ちなし)
	int32_t pendingActiveSlot_ = -1;
	// pendingActiveSlot_への送信済みフレーム数
	int32_t pendingSlotUpdateCount_ = 0;
};


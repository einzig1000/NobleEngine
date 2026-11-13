#pragma once
#include <Game.h>
#include <Utilities/PerlinNoise.h>
#include <definition/definition.h>
#include <GameObjects/Map/Terrain/TerrainDataManager/TerrainDataManager.h>

class FaceDataPagePool;
class ChunkGpuSlotPool;
struct ChunkGpuSlot;

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
	Chunk(const TerrainGenerator& generator, const Vector3int& chunkIndex, FaceDataPagePool* pagePool, ChunkGpuSlotPool* gpuPool);
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

	// 記録しておいた変更を書き戻す(生成直後、隣と接続する前に使う。隣のhaloには触らない)
	void ApplyEdit(const Vector3int& localIndex, BlockID id);

	/// <summary>
	/// 削除の準備。削除してよければ true
	/// (読み戻し待ち・直前のWrite.CSがGPUで実行中の間は false。毎フレーム呼べば、いずれ true になる)
	/// </summary>
	bool PrepareUnload();

	// 全部空気か(GPUスロットを使わないチャンク)
	bool IsAllAir() const { return isAllAir_; }
	// GPUスロットを借りているか
	bool HasGpuSlot() const { return gpu_ != nullptr; }


private:

	// 地形生成
	void CreateChunkData(const TerrainGenerator& generator);
	void CreateChunkDataFromJson();		// Jsonからチャンクデータを読み込み
	void CreateChunkDataNewly(const TerrainGenerator& generator);	// 新規生成チャンクデータ作成
	void GenerateOres(const NoiseParameter& param);	// 鉱石を生成
	void GenerateTrees(const NoiseParameter& param);// 木を生成

	// Count.CSの起動・ReadBack申請
	void DispatchCountPhase();
	// 総面数ReadBack取得・ページ確保・Write.CSの起動
	void FinishBakeWithPageAssignment(uint32_t totalFaces);
	// 総面数ReadBack取得・ページ確保・Write.CSの起動
	//void FinishBakeWithPageAssignment(uint32_t totalFaces, int64_t frameNumber);
	// 描画スロットを切り替え、今まで描画していたページを返す
	void SwitchActiveSlot(int32_t newSlot);


	// 最後のWrite.CSをGPUが実行し終えたか
	bool IsLastWriteFinished() const;


private:
	// 隣接チャンク static_cast<size_t>(DirectionXYZ)でアクセス
	Chunk* neighbors_[6] = {};
	// 全チャンク共有の面ページプール
	FaceDataPagePool* pagePool_ = nullptr;
	// 全チャンク共有のGPUスロットプール
	ChunkGpuSlotPool* gpuPool_ = nullptr;
	// 借りているGPUスロット(全部空気の間は nullptr)
	ChunkGpuSlot* gpu_ = nullptr;


	Matrix4x4 viewProjectionMatrix_;

	// チャンク情報
	Vector3int chunkIndex_;
	ChunkInfo chunkInfo_;
	AABB chunkAABB_;
	// ブロックデータ配列
	BlockID blocks_[Constexprs::kChunkBlockCountX][Constexprs::kChunkBlockCountY][Constexprs::kChunkBlockCountZ];
	uint32_t blockIds_[Constexprs::kMaxBlocksPerChunkPlusHalo];
	// ブロックID配列更新フラグ
	bool blockIdsDirty_ = false;
	// 全部空気か
	bool isAllAir_ = true;


	// ベイクの進行状況
	BakeState bakeState_ = BakeState::Idle;
	// 進行中の読み戻しトークン
	int32_t pendingReadbackToken_ = -1;
	// Count.CSに読ませたblockIdsのSRV。Write.CSにも同じものを読ませる
	uint32_t bakeBlockIdsSrv_ = UINT32_MAX;
	// 最後にWrite.CSを積んだフレーム(Game::Time::GetElapsedFrameTime() の値)
	uint32_t lastWriteFrame_ = 0;


	// 描画に使っているスロット番号(0 or 1)。Bakeは常にもう片方のスロットに対して行う
	int32_t activeSlot_ = 0;
	// このチャンクが現在保有しているページ番号一覧
	std::vector<uint32_t> myPages_[2];

	bool inCamera_ = false;
};


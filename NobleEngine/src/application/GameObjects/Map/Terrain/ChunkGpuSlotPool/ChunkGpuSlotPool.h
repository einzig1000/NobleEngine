#pragma once
#include <Game.h>
#include <memory>
#include <vector>
#include <cstdint>

class FaceDataPagePool;

// チャンク1個分のGPUリソースのID ＋ 描画・計算オブジェクト
struct ChunkGpuSlot
{
	// このチャンクのブロックID配列のリソースID
	int32_t blockIDsResourceID = -1;
	// グループごとの面数のリソースID
	int32_t faceCountResourceID[Constexprs::kFrameCount] = { -1, -1 };
	// グループごとの書き込み開始オフセットのリソースID
	int32_t groupOffsetResourceID[Constexprs::kFrameCount] = { -1, -1 };
	// 面カウンタ(InterlockedAdd用、1要素だけ)のリソースID
	int32_t faceWriteCounterResourceID = -1;
	// 保有ページ番号一覧のリソースID(可変長)
	int32_t myPagesResourceID[Constexprs::kFrameCount] = { -1, -1 };

	// 描画オブジェクト
	std::unique_ptr<RenderObject> render;
	// Count.CS用の計算オブジェクト
	std::unique_ptr<ComputeObject> countCompute;
	// Write.CS用の計算オブジェクト
	std::unique_ptr<ComputeObject> writeCompute;
};

// ChunkGpuSlotのマネージャー
class ChunkGpuSlotPool
{
public:
	ChunkGpuSlotPool(FaceDataPagePool* pagePool);
	~ChunkGpuSlotPool();
	void BeginFrame();

	/// <summary>
	/// スロットを借りる。
	/// 空きスロットが無い && フレームの新規作成上限 ならnullptr
	/// </summary>
	ChunkGpuSlot* TryGetFreeSlot();

	/// <summary>
	/// スロットを返却する
	/// </summary>
	void Release(ChunkGpuSlot* slot);

	/// <summary>
	/// 新たにfreeCount個のスロットを作成する
	/// </summary>
	void CreateSlots(size_t freeCount);

	// 総スロット数取得
	size_t GetTotalCount() const { return slots_.size(); }
	// 空きスロット数取得
	size_t GetFreeCount() const { return freeSlots_.size(); }

private:
	// 1フレームに作っていい最大数
	static constexpr int32_t kMaxCreatePerFrame = 2;


	// 新しいスロットを作る
	std::unique_ptr<ChunkGpuSlot> CreateSlot();


	// 面データページプール参照
	FaceDataPagePool* pagePool_ = nullptr;


	// 作成済みスロットリスト
	std::vector<std::unique_ptr<ChunkGpuSlot>> slots_;
	// 空きスロットリスト
	std::vector<ChunkGpuSlot*> freeSlots_;
	// このフレームに新しく作った数
	int32_t createdThisFrame_ = 0;
};

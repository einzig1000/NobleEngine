#pragma once
#include <vector>
#include <cstdint>
#include <Game.h>

class SharedFaceBucketPool
{
public:
	// 1スロットあたりの面数
	static constexpr uint32_t kBucketSize = 48;

	// 何スロット確保するか
	SharedFaceBucketPool(uint32_t totalBucketCount);

	/// <summary>
	/// 要求するスロット数を確保する。足りなかったら確保できた分だけ返す
	/// </summary>
	/// <param name="count">要求スロット数</param>
	/// <returns>確保したスロットのリスト</returns>
	std::vector<uint32_t> ClaimBuckets(uint32_t count);

	/// <summary>
	/// スロットを解放する
	/// </summary>
	/// <param name="buckets">解放するスロットのリスト</param>
	void ReleaseBuckets(const std::vector<uint32_t>& buckets);

	// CS/MSに渡すための共有バッファのヒープスロット
	int32_t GetSharedBufferHeapSlot() const { return sharedBakedFacesHeapSlot_; }

	// デバッグ表示用
	uint32_t GetTotalBucketCount() const { return totalBucketCount_; }
	uint32_t GetFreeBucketCount() const { return static_cast<uint32_t>(freeBuckets_.size()); }

private:
	uint32_t totalBucketCount_;
	std::vector<uint32_t> freeBuckets_;
	int32_t sharedBakedFacesHeapSlot_ = -1;
};
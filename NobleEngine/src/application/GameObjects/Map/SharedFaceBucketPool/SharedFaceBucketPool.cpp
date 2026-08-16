#include "SharedFaceBucketPool.h"

SharedFaceBucketPool::SharedFaceBucketPool(uint32_t totalBucketCount)
	: totalBucketCount_(totalBucketCount)
{
	// 共有バッファを一括確保(サイズ = バケツ数 × バケツサイズ × BakedFace1個分)
	sharedBakedFacesHeapSlot_ = Game::Resource::CreateCompute(sizeof(uint32_t) * 3, static_cast<size_t>(totalBucketCount_) * kBucketSize);

	// 最初は全バケツが空き
	freeBuckets_.reserve(totalBucketCount_);
	for (uint32_t i = 0; i < totalBucketCount_; ++i)
	{
		freeBuckets_.push_back(i);
	}
}

std::vector<uint32_t> SharedFaceBucketPool::ClaimBuckets(uint32_t count)
{
	// 要求数より空きが少なければ、あるだけ返す(呼び出し側が少ない数で対応する)
	uint32_t actualCount = std::min(count, static_cast<uint32_t>(freeBuckets_.size()));

	std::vector<uint32_t> result;
	result.reserve(actualCount);

	for (uint32_t i = 0; i < actualCount; ++i)
	{
		result.push_back(freeBuckets_.back());
		freeBuckets_.pop_back();
	}

	return result;
}

void SharedFaceBucketPool::ReleaseBuckets(const std::vector<uint32_t>& buckets)
{
	freeBuckets_.insert(freeBuckets_.end(), buckets.begin(), buckets.end());
}
#include "FaceDataPagePool.h"
#include <algorithm>

FaceDataPagePool::FaceDataPagePool(uint32_t totalPageCount)
	: totalPageCount_(totalPageCount)
{
	// 共有バッファを一括確保(サイズ = ページ数 × ページサイズ × BakedFace1個分)
	sharedBakedFacesResourceID_ = Game::Resource::CreateCompute(sizeof(uint32_t) * 3, static_cast<size_t>(totalPageCount_) * kPageSize);

	// 最初は全ページが空き
	freePages_.reserve(totalPageCount_);
	for (uint32_t i = 0; i < totalPageCount_; ++i)
	{
		freePages_.insert(i);
	}
}

std::vector<uint32_t> FaceDataPagePool::ClaimPages(uint32_t count)
{
	// 要求数より空きが少なければあるだけ返す
	uint32_t actualCount = std::min(count, static_cast<uint32_t>(freePages_.size()));

	std::vector<uint32_t> result;
	result.reserve(actualCount);

	for (uint32_t i = 0; i < actualCount; ++i)
	{
		auto it = freePages_.begin();
		result.push_back(*it);
		freePages_.erase(it);
	}

	return result;
}

void FaceDataPagePool::ReleasePages(const std::vector<uint32_t>& pages)
{
	freePages_.insert(pages.begin(), pages.end());
}
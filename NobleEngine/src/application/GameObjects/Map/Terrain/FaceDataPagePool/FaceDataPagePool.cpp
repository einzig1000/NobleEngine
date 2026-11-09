#include "FaceDataPagePool.h"
#include <algorithm>

FaceDataPagePool::FaceDataPagePool(uint32_t totalPageCount)
	: totalPageCount_(totalPageCount)
{
	// 共有バッファを一括確保(サイズ = 確保したいページ数 × ページサイズ × BakedFace1個分)
	sharedBakedFacesResourceID_ = Game::Resource::CreateCompute(sizeof(uint32_t) * 3, static_cast<size_t>(totalPageCount_) * kPageSize);

	// 最初は全ページが空き
	freePages_.resize(totalPageCount_);
	for (uint32_t i = 0; i < totalPageCount_; ++i)
	{
		// (使ったページ番号の最大値 + 1) == (使用ページ数の最大値)になるように末尾から入れる
		freePages_[i] = totalPageCount_ - 1 - i;
	}

}

FaceDataPagePool::~FaceDataPagePool()
{
	Game::Resource::Destroy(sharedBakedFacesResourceID_);
}

std::vector<uint32_t> FaceDataPagePool::ClaimPages(uint32_t count)
{
	// 足りなかったら空配列を返す
	if (count == 0 || count > freePages_.size()) return {};

	// 末尾から要求数分遡ったイテレータを取得
	const auto first = freePages_.end() - static_cast<std::ptrdiff_t>(count);
	// firstから末尾までのページ番号をコピー
	std::vector<uint32_t> result(first, freePages_.end());
	// firstから末尾までのページ番号をfreePages_から削除
	freePages_.erase(first, freePages_.end());

	// ピーク使用ページ数を更新
	peakUsedPageCount_ = std::max(peakUsedPageCount_, GetUsedPageCount());


	return result;
}

void FaceDataPagePool::Release(const std::vector<uint32_t>& pages)
{
	freePages_.insert(freePages_.end(), pages.begin(), pages.end());
}

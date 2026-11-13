#pragma once
#include <vector>
#include <unordered_set>
#include <cstdint>
#include <Game.h>

class FaceDataPagePool
{
public:
	// 1ページあたりの面数
	static constexpr uint32_t kPageSize = 48;

	// 何ページ確保するか
	FaceDataPagePool(uint32_t totalPageCount);
	~FaceDataPagePool();

	/// <summary>
	/// 要求するページ数を確保する。足りなかったら空配列を返す
	/// </summary>
	/// <param name="count">要求ページ数</param>
	/// <returns>確保したページのリスト</returns>
	std::vector<uint32_t> ClaimPages(uint32_t count);

	/// <summary>
	/// ページを解放する
	/// </summary>
	/// <param name="pages">解放するページのリスト</param>
	void Release(const std::vector<uint32_t>& pages);

	// CS/MSに渡すための共有バッファのリソースID
	int32_t GetSharedResourceID() const { return sharedBakedFacesResourceID_; }

	// 何ページ確保しているか
	uint32_t GetTotalPageCount() const { return totalPageCount_; }
	// 何ページ空いているか
	uint32_t GetFreePageCount() const { return static_cast<uint32_t>(freePages_.size()); }
	// 何ページ使っているか
	uint32_t GetUsedPageCount() const { return totalPageCount_ - GetFreePageCount(); }
	// 起動後最も多くページを使用していた瞬間の合計ページ数
	uint32_t GetPeakUsedPageCount() const { return peakUsedPageCount_; }


private:
	// 総ページ数
	uint32_t totalPageCount_;
	// 空きページリスト
	std::vector<uint32_t> freePages_;
	// リソースID
	int32_t sharedBakedFacesResourceID_ = -1;
	// 起動後最も多くページを使用していた瞬間の合計ページ数
	uint32_t peakUsedPageCount_ = 0;
};

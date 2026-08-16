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

	/// <summary>
	/// 要求するページ数を確保する。足りなかったら確保できた分だけ返す
	/// </summary>
	/// <param name="count">要求ページ数</param>
	/// <returns>確保したページのリスト</returns>
	std::vector<uint32_t> ClaimPages(uint32_t count);

	/// <summary>
	/// ページを解放する
	/// </summary>
	/// <param name="pages">解放するページのリスト</param>
	void ReleasePages(const std::vector<uint32_t>& pages);

	// CS/MSに渡すための共有バッファのリソースID
	int32_t GetSharedBufferResourceID() const { return sharedBakedFacesResourceID_; }

	// デバッグ表示用
	uint32_t GetTotalPageCount() const { return totalPageCount_; }
	uint32_t GetFreePageCount() const { return static_cast<uint32_t>(freePages_.size()); }

private:
	uint32_t totalPageCount_;
	std::unordered_set<uint32_t> freePages_;
	int32_t sharedBakedFacesResourceID_ = -1;
};

#pragma once
#include <cstdint>

namespace Constexprs
{
	static constexpr uint32_t kFrameCount = 2; // ダブルバッファリング


	static constexpr int32_t kChunkX = 64; // チャンクのX方向のブロック数
	static constexpr int32_t kChunkY = 64; // チャンクのY方向のブロック数
	static constexpr int32_t kChunkZ = 64; // チャンクのZ方向のブロック数

	// Y軸のチャンクを何段積み上げるか
	static constexpr int32_t kChunkStackHeight = 6;

	static constexpr float kBlockSize = 0.1f; // ブロックのサイズ
}
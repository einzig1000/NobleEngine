#pragma once
#include <cstdint>

namespace Constexprs
{
	static constexpr uint32_t kFrameCount = 2; // ダブルバッファリング


	static constexpr int32_t kChunkX = 16; // チャンクのX方向のブロック数
	static constexpr int32_t kChunkY = 16; // チャンクのY方向のブロック数
	static constexpr int32_t kChunkZ = 16; // チャンクのZ方向のブロック数
	static constexpr float kBlockSize = 0.1f; // ブロックのサイズ
}
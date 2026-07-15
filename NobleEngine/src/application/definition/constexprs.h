#pragma once
#include <cstdint>

namespace Constexprs
{
	static constexpr int32_t kChunkX = 16; // チャンクのX方向のブロック数
	static constexpr int32_t kChunkY = 16; // チャンクのY方向のブロック数
	static constexpr int32_t kChunkZ = 16; // チャンクのZ方向のブロック数

	// Y軸のチャンクを何段積み上げるか
	static constexpr int32_t kChunkStackHeight = 6;

	static constexpr float kBlockSize = 0.1f; // ブロックのサイズ

	// 重力
	static constexpr float GRAVITY = -0.01f;
}

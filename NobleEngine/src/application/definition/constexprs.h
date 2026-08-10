#pragma once
#include <cstdint>

namespace Constexprs
{
	static constexpr int32_t kChunkX = 32; // チャンクのX方向のブロック数
	static constexpr int32_t kChunkY = 64; // チャンクのY方向のブロック数
	static constexpr int32_t kChunkZ = 32; // チャンクのZ方向のブロック数

	// Y軸のチャンクを何段積み上げるか
	static constexpr int32_t kChunkStackHeight = 6;

	static constexpr float kBlockSize = 0.1f; // ブロックのサイズ

	// ワールド座標→ブロック番号変換時、kBlockSizeがfloatで正確に表現できないことによる丸め誤差(番号がブロック境界のわずかに手前に落ちて1小さくなる)を防ぐための微小値
	static constexpr float kBlockIndexEpsilon = 0.0001f;

	// 重力
	static constexpr float GRAVITY = -0.005f;
}

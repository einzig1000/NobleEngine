#pragma once
#include <cstdint>

namespace Constexprs
{
	static constexpr int32_t kChunkBlockCountX = 32;	// チャンクのX方向のブロック数
	static constexpr int32_t kChunkBlockCountY = 64;	// チャンクのY方向のブロック数
	static constexpr int32_t kChunkBlockCountZ = 32;	// チャンクのZ方向のブロック数
	static constexpr int32_t kChunkStackHeight = 6;		// Y軸のチャンクを何段積み上げるか


	// チャンク内のグループ数(1グループ = 2x2x2のブロック群。１メッシュレットとして扱える)
	static constexpr uint32_t kGroupCount = (kChunkBlockCountX * kChunkBlockCountY * kChunkBlockCountZ) / 8;
	// １グループ(8ブロック)の最大面数
	static constexpr uint32_t kMaxFacesPerGroup = 48;
	// １チャンクの最大面数
	static constexpr uint32_t kMaxFacesPerChunk = (kGroupCount * kMaxFacesPerGroup) / 48;
	// 1チャンク + 全方向１ブロック の最大ブロック数
	static constexpr uint32_t kMaxFacesPerChunkPlusHalo = (kChunkBlockCountX + 2) * (kChunkBlockCountY + 2) * (kChunkBlockCountZ + 2);


	static constexpr float kBlockSize = 0.1f; // ブロックのサイズ

	// ワールド座標→ブロック番号変換時、kBlockSizeがfloatで正確に表現できないことによる丸め誤差(番号がブロック境界のわずかに手前に落ちて1小さくなる)を防ぐための微小値
	static constexpr float kBlockIndexEpsilon = 0.0001f;

	// 重力
	static constexpr float GRAVITY = -0.005f;
}

#pragma once
#include <cstdint>

namespace Constexprs
{
	static constexpr uint32_t kFrameCount = 2; // ダブルバッファリング

	static constexpr uint32_t kInitialWindowWidth = 1280;
	static constexpr uint32_t kInitialWindowHeight = 720;

	static constexpr float eps = 1e-6f;
}

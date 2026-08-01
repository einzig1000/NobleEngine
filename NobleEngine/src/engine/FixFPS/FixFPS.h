#pragma once
#include <chrono>

/// <summary>
/// FPS制御クラス
/// </summary>
class FixFPS
{
public:
	FixFPS();
	void Update();

	float GetClampedDeltaTimeMs() const { return clampedDeltaMs_; }
	float GetClampedFPS() const { return clampedFPS_; }
	float GetUnclampedFPS() const { return unclampedFPS_; }
	float GetUnclampedDeltaTimeMs() const { return unclampedDeltaMs_; }
	void SetTargetFPS(int32_t targetFPS);
	uint32_t GetFrameCount() const { return frameCount_; }

private:
	std::chrono::steady_clock::time_point previousTime_;
	// 目標FPS
	int32_t targetFPS_ = 60;
	std::chrono::microseconds targetFrameDuration_;
	// 実際のFPS
	float clampedFPS_ = 0.0f;
	float clampedDeltaMs_ = 0.0f;
	// 理論値FPS
	float unclampedFPS_ = 0.0f;
	float unclampedDeltaMs_ = 0.0f;
	// 起動後経過時間
	uint32_t frameCount_ = 0;
};

#pragma once
#include <chrono>

/// <summary>
/// FPS制御クラス
/// </summary>
class FixFPS
{
public:
	FixFPS();
	void UpdateFixFPS();
	float GetDeltaTime() const { return deltaTime_; }
	float GetRealFPS() const { return realTimeFPS_; }
	float GetAverageFPS() const { return averageFPS_; }
	void SetFPS(int32_t targetFPS) { targetFPS_ = targetFPS; }
	int32_t& GetTargetFpsPtr() { return targetFPS_; }
	uint32_t GetFrameCount() const { return frameCount_; }
	float GetMaxFPS() const { return maxFPS_; }

private:
	std::chrono::steady_clock::time_point previousTime_;
	int32_t targetFPS_;
	float maxFPS_ = 0;
	float averageMaxFPS_ = 0;

	float deltaTime_;

	float averageFPS_;
	float realTimeFPS_;

	uint32_t frameCount_ = 0;
};
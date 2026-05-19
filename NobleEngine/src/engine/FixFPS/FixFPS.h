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
	float GetTimeScale() const { return timeScale_; }
	void SetTimeScale(float timeScale) { timeScale_ = timeScale; }
	uint32_t GetFrameCount() const { return frameCount_; }

private:
	std::chrono::steady_clock::time_point previousTime_;
	float targetFPS_;

	float deltaTime_;

	float averageFPS_;
	float realTimeFPS_;

	float timeScale_ = 1.0f;

	uint32_t frameCount_ = 0;
};
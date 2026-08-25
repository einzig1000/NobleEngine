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

	/// <returns>最大60FPSにクランプされたデルタタイム</returns>
	float GetClampedDeltaTimeMs() const { return clampedDeltaMs_; }
	/// <returns>最大60FPSにクランプされたFPS</returns>
	float GetClampedFPS() const { return clampedFPS_; }
	/// <returns>60FPS固定にされていなかったら出ていたデルタタイム</returns>
	float GetUnclampedDeltaTimeMs() const { return unclampedDeltaMs_; }
	/// <returns>60FPS固定にされていなかったら出ていたFPS</returns>
	float GetUnclampedFPS() const { return unclampedFPS_; }
	/// 最大FPSを設定する
	void SetFPSCap(int32_t fpsCap);
	/// <returns> 最大FPS</returns>
	int32_t GetFPSCap() const { return fpsCap_; }
	/// <returns>起動後経過時間</returns>
	uint32_t GetFrameCount() const { return frameCount_; }

private:
	std::chrono::steady_clock::time_point previousTime_;
	// 目標FPS
	int32_t fpsCap_ = 60;
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

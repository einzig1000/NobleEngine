#include "FixFPS/FixFPS.h"
#include <thread>

FixFPS::FixFPS()
{
	previousTime_ = std::chrono::steady_clock::now();

	SetFPSCap(60);
}

void FixFPS::Update()
{
    // 現在の時間を取得
    std::chrono::steady_clock::time_point currentTime =
        std::chrono::steady_clock::now();

	// 経過時間を計算(1フレームにかかった時間)
    std::chrono::microseconds elapsedTime =
        std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime_);

	// 実際の経過時間から理論値FPSを計算
    double deltaMs = std::chrono::duration<double, std::milli>(elapsedTime).count();
    unclampedDeltaMs_ = static_cast<float>(deltaMs);
    unclampedFPS_ = static_cast<float>(1000.0 / deltaMs);

    // 目標フレーム時間に満たないなら待機
    if (elapsedTime < targetFrameDuration_)
    {
        // 目標時間
        const auto target = previousTime_ + targetFrameDuration_;
        // 睡眠時間を計算
        auto sleepDuration = std::chrono::duration_cast<std::chrono::microseconds>(target - currentTime);
        // sleepDurationが2000以上の場合のみスリープ
        if (sleepDuration.count() > 2000)
        {
            std::this_thread::sleep_for(sleepDuration - std::chrono::microseconds(2000));
        }
		// スリープ後の現在の時間を再取得
        currentTime = std::chrono::steady_clock::now();
		// ジャストまで待機
        while (currentTime < target)
        {
            currentTime = std::chrono::steady_clock::now();
        }
    }
    // 目標時間を超過している場合はそのまま進行
    else
    {
        currentTime = std::chrono::steady_clock::now();
    }

    // 待機を含めた経過時間からFPSを計算
    elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime_);
    deltaMs = std::chrono::duration<double, std::milli>(elapsedTime).count();
    clampedDeltaMs_ = static_cast<float>(deltaMs);
    clampedFPS_ = static_cast<float>(1000.0 / deltaMs);

    // 前回時間を更新
    previousTime_ = currentTime;

    // フレームカウント更新
    frameCount_++;
}

void FixFPS::SetFPSCap(int32_t fpsCap)
{
	fpsCap_ = fpsCap;
	double targetFrameTime = 1.0 / static_cast<double>(fpsCap_);
    targetFrameDuration_ = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(targetFrameTime));
}
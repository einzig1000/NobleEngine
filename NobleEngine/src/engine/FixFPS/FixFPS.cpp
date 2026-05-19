#include "FixFPS/FixFPS.h"
#include <thread>
#include <algorithm>

FixFPS::FixFPS()
{
	previousTime_ = std::chrono::steady_clock::now();

    deltaTime_ = 0.0f;

	averageFPS_ = 0.0f;
	realTimeFPS_ = 0.0f;

    targetFPS_ = 60;
}

void FixFPS::UpdateFixFPS()
{
    // {targetFPS_(基本60) / 1.0 秒} をマイクロ秒に変換 → 1フレーム = kFrame
    const std::chrono::microseconds kFrame =
        std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(1.0 / targetFPS_));

    // 現在の時間を取得
    std::chrono::steady_clock::time_point currentTime =
        std::chrono::steady_clock::now();

    // 経過時間を計算
    std::chrono::microseconds elapsedTime =
        std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime_);

    // 目標フレーム時間に満たないなら待機
    if (elapsedTime < kFrame)
    {
        // 目標時間
        const auto target = previousTime_ + kFrame;
        // 睡眠時間を計算
        auto sleepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(target - currentTime);
        // sleepDurationが0以上の場合のみスリープ
        if (sleepDuration.count() > 0)
        {
            std::this_thread::sleep_for(sleepDuration);
        }
    }
    // 目標時間を超過している場合はそのまま進行
    else
    {
        currentTime = std::chrono::steady_clock::now();
    }

    // 待機を含めた実フレーム時間で確定
    deltaTime_ = std::chrono::duration<float>(currentTime - previousTime_).count();
    if (deltaTime_ != 0)realTimeFPS_ = (1.0f / deltaTime_);
    averageFPS_ = (averageFPS_ * 0.95f) + (realTimeFPS_ * 0.05f);
	deltaTime_ = std::clamp(deltaTime_, 0.0f, 1.0f);

    // 前回時間を更新
    previousTime_ = currentTime;

    // フレームカウント更新
    frameCount_++;
}
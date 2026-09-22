#include "KeyboardController.h"
#include <Engine.h>
#include <TimeManager/TimeManager.h>

KeyboardController::KeyboardController(HWND hwnd)
    : hwnd_(hwnd)
{
    for (int32_t i = 0; i < 256; ++i)
    {
        keys_[i] = KeyState{};
    }
}

void KeyboardController::Update()
{
    const bool hasFocus = (GetForegroundWindow() == hwnd_);
    const float deltaSeconds = Engine::Instance().GetTimeManager()->GetScaledDeltaTimeMs() * 0.001f;

    for (int32_t k = 0; k < 256; ++k)
    {
        bool pressed = hasFocus && ((GetAsyncKeyState(k) & 0x8000) != 0);

        KeyState& ks = keys_[k];
        ks.prev = ks.curr;
        ks.curr = pressed;

        if (ks.curr)
        {
            ks.holdSeconds += deltaSeconds;

            //// 押されている → 連続フレーム数を増やす
            //if (ks.prev)
            //{
            //    // 前フレームも押されていた
            //    ++ks.holdFrames;
            //}
            //else
            //{
            //    // 押し始め（edge）
            //    ks.holdFrames = 1;
            //}
        }
        else
        {
            // 離されたフレーム
            if (ks.prev && !ks.curr)
            {
                // 保存して holdFrames をリセット
                ks.lastHoldOnRelease = ks.holdSeconds;
            }
            ks.holdSeconds = 0.0f;
        }
    }
}

bool KeyboardController::IsHeld(BYTE key) const
{
    return keys_[key].curr;
}

bool KeyboardController::IsJustPressed(BYTE key) const
{
    return (!keys_[key].prev && keys_[key].curr);
}

bool KeyboardController::IsJustReleased(BYTE key) const
{
    return (keys_[key].prev && !keys_[key].curr);
}

float KeyboardController::HoldSeconds(BYTE key) const
{
    return keys_[key].holdSeconds;
}

// 0: なし  1:単押し  2:長押し(n = 長押し判定)
uint32_t KeyboardController::TestTapLong(float thresholdSeconds, BYTE key) const
{
    const KeyState& ks = keys_[key];

    // 離された瞬間フレーム
    if (ks.prev && !ks.curr)
    {
        if (ks.lastHoldOnRelease < thresholdSeconds) return 1;     // Tap
        else return 2;                              // Long
    }

	// 押してる最中
    if (ks.curr)
    {
        if (ks.holdSeconds >= thresholdSeconds)     // 押してからn秒以上経過 -> Long確定
        return 0;                                   // 継続中だがまだ n 未満 -> 未確定
    }

    return 0;
}

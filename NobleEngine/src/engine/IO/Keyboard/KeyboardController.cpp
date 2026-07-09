#include "KeyboardController.h"

KeyboardController::KeyboardController(HWND hwnd)
    : hwnd_(hwnd)
{
    // 初期化
    for (int32_t i = 0; i < 256; ++i)
    {
        keys_[i] = KeyState{};
    }
}

void KeyboardController::Update()
{
    // 元の DISCL_FOREGROUND 相当の挙動を再現するため、
    // フォアグラウンドでないときは全キー解放扱いにする。
    // （不要ならこの判定は外してよい）
    const bool hasFocus = (GetForegroundWindow() == hwnd_);

    for (int32_t k = 0; k < 256; ++k)
    {
        bool pressed = hasFocus && ((GetAsyncKeyState(k) & 0x8000) != 0);

        KeyState& ks = keys_[k];
        ks.prev = ks.curr;
        ks.curr = pressed;

        if (ks.curr)
        {
            // 押されている → 連続フレーム数を増やす
            if (ks.prev)
            {
                // 前フレームも押されていた
                ++ks.holdFrames;
            }
            else
            {
                // 押し始め（edge）
                ks.holdFrames = 1;
            }
        }
        else
        {
            // 離されたフレーム
            if (ks.prev && !ks.curr)
            {
                // release event: 保存して holdFrames をリセット
                ks.lastHoldOnRelease = ks.holdFrames;
            }
            ks.holdFrames = 0;
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

uint32_t KeyboardController::HoldFrames(BYTE key) const
{
    return keys_[key].holdFrames;
}

// 0: なし  1:単押し  2:長押し(n = 長押し判定)
uint32_t KeyboardController::TestTapLong(uint32_t n, BYTE key) const
{
    const KeyState& ks = keys_[key];

    // 1) 押してから n フレーム以内に離したら Tap (1)
    //    => release が発生していて、 lastHoldOnRelease が 1..(n-1)
    if (ks.prev && !ks.curr)
    {
        // 直前が押されていて今リリース（release イベント）
        if (ks.lastHoldOnRelease > 0)
        {
            if (ks.lastHoldOnRelease < n) return 1;     // Tap
            else return 2;                                   // Long (>= n)
        }
        // fallthrough -> 0
        return 0;
    }

    // 2) まだ押下継続中なら、現在の holdFrames をチェック
    if (ks.curr)
    {
        if (ks.holdFrames >= n) return 2; // 押してから n フレーム以上経過 -> Long
        return 0; // 継続中だがまだ n 未満 -> 未確定
    }

    // 3) 何もない
    return 0;
}

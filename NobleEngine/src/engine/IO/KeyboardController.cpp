#include "IO/KeyboardController.h"
#include <cassert>

KeyboardController::KeyboardController(HWND hwnd)
{
    Microsoft::WRL::ComPtr<IDirectInput8> directInput;
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    HRESULT result = DirectInput8Create(
        hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
        reinterpret_cast<void**>(directInput.GetAddressOf()),
        nullptr
    );
    assert(SUCCEEDED(result));

    result = directInput->CreateDevice(GUID_SysKeyboard, keyboard_.GetAddressOf(), NULL);
    assert(SUCCEEDED(result));

    result = keyboard_->SetDataFormat(&c_dfDIKeyboard);
    assert(SUCCEEDED(result));

    result = keyboard_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
    assert(SUCCEEDED(result));

    // 初期化
    for (int i = 0; i < 256; ++i)
    {
        diKeys_[i] = 0;
    }

}

void KeyboardController::Update()
{
    if (keyboard_)
    {
        keyboard_->Acquire();
        HRESULT hr = keyboard_->GetDeviceState(sizeof(diKeys_), diKeys_);
        if (FAILED(hr))
        {
            // Acquire 失敗やフォーカス喪失時はキー全開放扱いにする
            for (int k = 0; k < 256; ++k)
            {
                keys_[k].prev = keys_[k].curr;
                // treat as released
                keys_[k].curr = false;
                if (keys_[k].prev && !keys_[k].curr)
                {
                    // release event: 保存
                    keys_[k].lastHoldOnRelease = keys_[k].holdFrames;
                }
                keys_[k].holdFrames = 0;
            }
            return;
        }

        for (int k = 0; k < 256; ++k)
        {
            bool pressed = (diKeys_[k] & 0x80) != 0;

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
int KeyboardController::TestTapLong(int n, BYTE key) const
{
    const KeyState& ks = keys_[key];

    // 1) 押してから n フレーム以内に離したら Tap (1)
    //    => release が発生していて、 lastHoldOnRelease が 1..(n-1)
    if (ks.prev && !ks.curr)
    {
        // 直前が押されていて今リリース（release イベント）
        if (ks.lastHoldOnRelease > 0)
        {
            if ((int)ks.lastHoldOnRelease < n) return 1;     // Tap
            else return 2;                                   // Long (>= n)
        }
        // fallthrough -> 0
        return 0;
    }

    // 2) まだ押下継続中なら、現在の holdFrames をチェック
    if (ks.curr)
    {
        if ((int)ks.holdFrames >= n) return 2; // 押してから n フレーム以上経過 -> Long
        return 0; // 継続中だがまだ n 未満 -> 未確定
    }

    // 3) 何もない
    return 0;
}

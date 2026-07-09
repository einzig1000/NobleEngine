#pragma once
#include <windows.h>
#include "definition/definition.h"

// DirectInput は Windows 8 以降 非推奨のため、
// Win32 標準の GetAsyncKeyState に置き換えたバージョン。
// dinput8.lib / dxguid.lib のリンクは不要になる。

/// <summary>
/// キーボード管理クラス
/// </summary>
class KeyboardController
{
public:
	KeyboardController(HWND hwnd);
	void Update();

	// 今押しているか
	bool IsHeld(BYTE key) const;
	// 押した瞬間（今フレームで押された）
	bool IsJustPressed(BYTE key) const;
	// 離した瞬間（今フレームで離れた）
	bool IsJustReleased(BYTE key) const;

	// 押されてからの経過フレーム数
	uint32_t HoldFrames(BYTE key) const;

	// 0: なし  1:単押し  2:長押し(n = 長押し判定)
	uint32_t TestTapLong(uint32_t n, BYTE key) const;

private:

	KeyState keys_[256];

	// フォーカスが外れているフレームはキー全開放扱いにするために保持
	HWND hwnd_;
};

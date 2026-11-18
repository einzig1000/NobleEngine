#pragma once
#include <windows.h>
#include <EngineDefinition/EngineDefinition.h>

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
	// 押されてからの経過秒数
	float HoldSeconds(BYTE key) const;

	// 0: なし  1:単押し  2:長押し(thresholdSeconds = 長押し判定の閾値秒数)
	uint32_t TestTapLong(float thresholdSeconds, BYTE key) const;

private:

	KeyState keys_[256];

	HWND hwnd_;
};

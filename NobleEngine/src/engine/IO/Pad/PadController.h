#pragma once
#include <Windows.h>
#include "definition/definition.h"
#include <Xinput.h>
#pragma comment(lib, "xinput.lib") // XInputライブラリをリンク

/// <summary>
/// パッド管理クラス
/// </summary>
class PadController
{
public:
    PadController();
	~PadController();

    void Update();

	bool IsHeld(int padIndex, BYTE button) const;
	bool IsJustPressed(int padIndex, BYTE button) const;
	bool IsJustReleased(int padIndex, BYTE button) const;

    uint32_t HoldFrames(int padIndex, BYTE button) const;	// 押されてからの経過フレーム数
	Vector2 GetLeftStick(int padIndex) const;	    // 左スティックの値取得 (-1.0f ～ 1.0f)
	Vector2 GetRightStick(int padIndex) const;	// 右スティックの値取得 (-1.0f ～ 1.0f)

    // トリガーの値取得（0.0f ～ 1.0f）
    float GetLeftTrigger(int padIndex) const;
    float GetRightTrigger(int padIndex) const;
    // ゲームパッド振動
    void SetVibration(int padIndex, float leftMotor, float rightMotor);

    int32_t GetConnectedPadNum() const;

private:
    PadButtonState padStates[4][PAD_BUTTON_MAX]{};

    bool isConnect[4];

    BYTE leftTrigger[4];     // 左トリガー（0〜255）
    BYTE rightTrigger[4];    // 右トリガー（0〜255）

    SHORT leftStickX[4];        // 左スティックX軸（-32768〜32767）
    SHORT leftStickY[4];        // 左スティックY軸（-32768〜32767）
    SHORT rightStickX[4];        // 右スティックX軸（-32768〜32767）
    SHORT rightStickY[4];        // 右スティックY軸（-32768〜32767）
    Vector2 leftStickDir[4];        // 左スティック方向({-1〜1},{-1〜1})
    Vector2 rightStickDir[4];        // 右スティック方向({-1〜1},{-1〜1})



    XINPUT_STATE state[4];
    XINPUT_STATE preState[4];
    DWORD dwUserIndex[4];
};
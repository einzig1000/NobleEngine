#include "IO/PadController.h"
#include <algorithm>
#include <cstring>

PadController::PadController()
{
    for (int32_t i = 0; i < 4; ++i)
    {
        isConnect[i] = false;
    }

    {
        // 明示的初期化
        std::memset(state, 0, sizeof(state));
        std::memset(preState, 0, sizeof(preState));
        std::memset(isConnect, 0, sizeof(isConnect));
        std::memset(leftTrigger, 0, sizeof(leftTrigger));
        std::memset(rightTrigger, 0, sizeof(rightTrigger));
        std::memset(leftStickX, 0, sizeof(leftStickX));
        std::memset(leftStickY, 0, sizeof(leftStickY));
        std::memset(rightStickX, 0, sizeof(rightStickX));
        std::memset(rightStickY, 0, sizeof(rightStickY));
        for (int i = 0; i < 4; ++i)
        {
            leftStickDir[i] = { 0.0f, 0.0f };
            rightStickDir[i] = { 0.0f, 0.0f };
            for (int b = 0; b < PAD_BUTTON_MAX; ++b)
            {
                padStates[i][b].curr = false;
                padStates[i][b].prev = false;
                padStates[i][b].holdFrames = 0;
                padStates[i][b].lastHoldOnRelease = 0;
            }
        }
    }

}

PadController::~PadController()
{}

void PadController::Update()
{
    for (DWORD i = 0; i < 4; ++i)
    {
        preState[i] = state[i];

        ZeroMemory(&state[i], sizeof(XINPUT_STATE));
        if (XInputGetState(i, &state[i]) == ERROR_SUCCESS)
        {
            for (int32_t button = 0; button < PAD_BUTTON_MAX; ++button)
            {
                padStates[i][button].prev = padStates[i][button].curr;

                if (padStates[i][button].curr)
                {
                    ++padStates[i][button].holdFrames;
                }
                else
                {
                    if (padStates[i][button].prev)
                    {
                        padStates[i][button].lastHoldOnRelease = padStates[i][button].holdFrames;
                    }
                    padStates[i][button].holdFrames = 0;
                }
            }

            WORD b = state[i].Gamepad.wButtons;
            padStates[i][PAD_A].curr = (b & XINPUT_GAMEPAD_A) != 0;
            padStates[i][PAD_B].curr = (b & XINPUT_GAMEPAD_B) != 0;
            padStates[i][PAD_X].curr = (b & XINPUT_GAMEPAD_X) != 0;
            padStates[i][PAD_Y].curr = (b & XINPUT_GAMEPAD_Y) != 0;
            padStates[i][PAD_LB].curr = (b & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
            padStates[i][PAD_RB].curr = (b & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
            padStates[i][PAD_LS].curr = (b & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
            padStates[i][PAD_RS].curr = (b & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
            padStates[i][PAD_BACK].curr = (b & XINPUT_GAMEPAD_BACK) != 0;
            padStates[i][PAD_START].curr = (b & XINPUT_GAMEPAD_START) != 0;
            padStates[i][PAD_UP].curr = (b & XINPUT_GAMEPAD_DPAD_UP) != 0;
            padStates[i][PAD_DOWN].curr = (b & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
            padStates[i][PAD_LEFT].curr = (b & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
            padStates[i][PAD_RIGHT].curr = (b & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;

            leftTrigger[i] = state[i].Gamepad.bLeftTrigger;
            rightTrigger[i] = state[i].Gamepad.bRightTrigger;
            
            leftStickX[i] = state[i].Gamepad.sThumbLX;
            leftStickY[i] = state[i].Gamepad.sThumbLY;
            rightStickX[i] = state[i].Gamepad.sThumbRX;
            rightStickY[i] = state[i].Gamepad.sThumbRY;
            
            Vector2 leftStick = { float(leftStickX[i]), float(leftStickY[i]) };
			leftStickDir[i] = Vector2(leftStick.x / 32768.0f, leftStick.y / 32768.0f);
            Vector2 rightStick = { float(rightStickX[i]), float(rightStickY[i]) };
			rightStickDir[i] = Vector2(rightStick.x / 32768.0f, rightStick.y / 32768.0f);
            isConnect[i] = true;

        }
        else
        {
            isConnect[i] = false;
            for (int button = 0; button < PAD_BUTTON_MAX; ++button)
            {
                PadButtonState& ps = padStates[i][button];
                ps.prev = ps.curr;
                ps.curr = false;
                if (ps.prev) ps.lastHoldOnRelease = ps.holdFrames;
                ps.holdFrames = 0;
            }
            leftTrigger[i] = rightTrigger[i] = 0;
            leftStickX[i] = leftStickY[i] = 0;
            rightStickX[i] = rightStickY[i] = 0;
            leftStickDir[i] = { 0.0f, 0.0f };
            rightStickDir[i] = { 0.0f, 0.0f };

        }
    }
}

bool PadController::IsHeld(int padIndex, BYTE button) const
{
    int stateIndex = std::clamp(padIndex, 0, 3);
    int btn = std::clamp(int(button), 0, PAD_BUTTON_MAX - 1);
	return padStates[stateIndex][btn].curr;
}

bool PadController::IsJustPressed(int padIndex, BYTE button) const
{
    int stateIndex = std::clamp(padIndex, 0, 3);
    int btn = std::clamp(int(button), 0, PAD_BUTTON_MAX - 1);
	return (!padStates[stateIndex][btn].prev && padStates[stateIndex][btn].curr);
}

bool PadController::IsJustReleased(int padIndex, BYTE button) const
{
    int stateIndex = std::clamp(padIndex, 0, 3);
    int btn = std::clamp(int(button), 0, PAD_BUTTON_MAX - 1);
    return (padStates[stateIndex][btn].prev && !padStates[stateIndex][btn].curr);
}

uint32_t PadController::HoldFrames(int padIndex, BYTE button) const
{
    int stateIndex = std::clamp(padIndex, 0, 3);
    int btn = std::clamp(int(button), 0, PAD_BUTTON_MAX - 1);
    return padStates[stateIndex][btn].holdFrames;
}

Vector2 PadController::GetLeftStick(int padIndex) const
{
    int stateIndex = std::clamp(padIndex, 0, 3);
	return leftStickDir[stateIndex];
}

Vector2 PadController::GetRightStick(int padIndex) const
{
    int stateIndex = std::clamp(padIndex, 0, 3);
    return rightStickDir[stateIndex];
}

float PadController::GetLeftTrigger(int padIndex) const
{
    int stateIndex = std::clamp(padIndex, 0, 3);
	float value = static_cast<float>(leftTrigger[stateIndex]) / 255.0f;
	return value;
}

float PadController::GetRightTrigger(int padIndex) const
{
    int stateIndex = std::clamp(padIndex, 0, 3);
    float value = static_cast<float>(rightTrigger[stateIndex]) / 255.0f;
	return value;
}

void PadController::SetVibration(int padIndex, float leftMotor, float rightMotor)
{
    int stateIndex = std::clamp(padIndex, 0, 3);

    leftMotor = std::clamp(leftMotor, 0.0f, 1.0f);
    rightMotor = std::clamp(rightMotor, 0.0f, 1.0f);

    XINPUT_VIBRATION vib;
    ZeroMemory(&vib, sizeof(vib));
    // XInput uses 0..65535 for motor speed
    vib.wLeftMotorSpeed = static_cast<WORD>(leftMotor * 65535.0f);
    vib.wRightMotorSpeed = static_cast<WORD>(rightMotor * 65535.0f);

    XInputSetState(static_cast<DWORD>(stateIndex), &vib);
}

int32_t PadController::GetConnectedPadNum() const
{
    int sum = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (isConnect[i])
        {
            ++sum;
        }
    }
	return sum;
}

#pragma once
#include <memory>
#include <Windows.h>

class CameraManager;

class KeyboardController;
class PadController;
class MouseController;

/// <summary>
/// 入出力管理クラス
/// </summary>
class IOManager
{
public:
    IOManager(HWND hwnd, CameraManager* cameraManager);
    ~IOManager();
    void Update();
    void EndFrame();

    MouseController* GetMouseController() const { return mouseController_.get(); }
    KeyboardController* GetKeyboardController() const { return keyboardController_.get(); }
    PadController* GetPadController() const { return padController_.get(); }

private:

    std::unique_ptr<KeyboardController> keyboardController_;
    std::unique_ptr<PadController> padController_;
    std::unique_ptr<MouseController> mouseController_;
};
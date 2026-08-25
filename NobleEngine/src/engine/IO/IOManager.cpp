#include "IO/IOManager.h"
#include "IO/Keyboard/KeyboardController.h"
#include "IO/Pad/PadController.h"
#include "IO/Mouse/MouseController.h"

IOManager::IOManager(HWND hwnd)
{
    keyboardController_ = std::make_unique<KeyboardController>(hwnd);
    padController_ = std::make_unique<PadController>();
    mouseController_ = std::make_unique<MouseController>(hwnd);
}

IOManager::~IOManager(){}

void IOManager::Update()
{
    keyboardController_->Update();
    padController_->Update();
    mouseController_->Update();
}

void IOManager::EndFrame()
{
    mouseController_->EndFrame();
}
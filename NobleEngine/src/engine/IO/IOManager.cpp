#include "IO/IOManager.h"
#include "IO/KeyboardController.h"
#include "IO/PadController.h"
#include "IO/MouseController.h"

IOManager::IOManager(HWND hwnd, CameraManager* cameraManager)
{
    keyboardController_ = std::make_unique<KeyboardController>(hwnd);
    padController_ = std::make_unique<PadController>();
    mouseController_ = std::make_unique<MouseController>(hwnd, cameraManager);
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
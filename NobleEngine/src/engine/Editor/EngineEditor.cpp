#include "EngineEditor.h"
#include "AssetsEditor/AssetsEditor.h"
#include "TimeEditor/TimeEditor.h"
#include "GPUResourcePreview/GPUResourcePreview.h"
#include "LogPanel/LogPanel.h"

#include <IO/IOManager.h>
#include <Window/WindowManager.h>
#include <DirectX/DirectXManager.h>
#include <AssetManager/AssetManager.h>
#include <DrawSystem/DrawSystem.h>
#include <Camera/CameraManager.h>
#include <TimeManager/TimeManager.h>

EngineEditor::EngineEditor(
	WindowManager* windowManager,
	DirectXManager* dxManager,
	DrawSystem* drawSystem,
	IOManager* ioManager,
	CameraManager* cameraManager,
	AssetManager* assetManager,
	TimeManager* timeManager)
{
	panels_.push_back(std::make_unique<AssetsEditor>(dxManager, cameraManager, assetManager));
	panels_.push_back(std::make_unique<TimeEditor>(timeManager, dxManager->GetFrameProfiler()));
	panels_.push_back(std::make_unique<GPUResourcePreview>(dxManager->GetDescriptorHeapManager()));
	panels_.push_back(std::make_unique<LogPanel>());
}
EngineEditor::~EngineEditor()
{}

void EngineEditor::Initialize()
{}

void EngineEditor::Update()
{
	for (auto& panel : panels_)
	{
		panel->Update();
	}
}

void EngineEditor::Draw()
{
	for (auto& panel : panels_)
	{
		panel->Draw();
	}
}

void EngineEditor::DrawImGui()
{
	for (auto& panel : panels_)
	{
		panel->DrawImGui();
	}
}

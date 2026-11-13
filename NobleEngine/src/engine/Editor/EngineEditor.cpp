#include "EngineEditor.h"
#include "AssetsEditor/AssetsEditor.h"
#include "AssetsEditor/ModelPreview/ModelPreview.h"
#include "AssetsEditor/TexturePreview/TexturePreview.h"
#include "AssetsEditor/AudioPreview/AudioPreview.h"
#include "AssetsEditor/RenderTexturePreview/RenderTexturePreview.h"
#include "TimeEditor/TimeEditor.h"
#include "GPURespurcePreview/GPURespurcePreview.h"

#include <IO/IOManager.h>
#include <Window/WindowManager.h>
#include <DirectX/DirectXManager.h>
#include <AssetManager/AssetManager.h>
#include <DrawSystem/DrawSystem.h>
#include <Camera/CameraManager.h>
#include <TimeManager/TimeManager.h>
#include <ImGuiManager/ImGuiManager.h>

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
	panels_.push_back(std::make_unique<GPURespurcePreview>(dxManager->GetDescriptorHeapManager()));
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

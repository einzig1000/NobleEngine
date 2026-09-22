#include "EngineEditor.h"
#include "ModelPreview/ModelPreview.h"
#include "TexturePreview/TexturePreview.h"
#include "AudioPreview/AudioPreview.h"
#include "RenderTexturePreview/RenderTexturePreview.h"
#include "TimeEditor/TimeEditor.h"

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
	modelEditor_ = std::make_unique<ModelPreview>(dxManager, cameraManager, assetManager->GetModelManager()->GetModelBank());
	textureEditor_ = std::make_unique<TexturePreview>(dxManager, assetManager->GetTextureManager()->GetTextureBank());
	audioPreview_ = std::make_unique<AudioPreview>(assetManager->GetAudioManager()->GetAudioBank(), assetManager->GetAudioManager()->GetAudioPlayer());
	renderTexturePreview_ = std::make_unique<RenderTexturePreview>(dxManager);

	timeEditor_ = std::make_unique<TimeEditor>(timeManager, dxManager->GetFrameProfiler());
}

EngineEditor::~EngineEditor()
{}

void EngineEditor::Initialize()
{}

void EngineEditor::Update()
{
	modelEditor_->Update();
}

void EngineEditor::Draw()
{
	modelEditor_->Draw();
}

void EngineEditor::DrawImGui()
{
	modelEditor_->DrawImGui();
	textureEditor_->DrawImGui();
	audioPreview_->DrawImGui();
	renderTexturePreview_->DrawImGui();
	timeEditor_->DrawImGui();
}

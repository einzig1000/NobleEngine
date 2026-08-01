#include "EngineEditor.h"
#include "ModelPreview/ModelPreview.h"
#include "TexturePreview/TexturePreview.h"
#include "RenderTexturePreview/RenderTexturePreview.h"
#include "TimeEditor/TimeEditor.h"

#include <IO/IOManager.h>
#include <Window/WindowManager.h>
#include <DirectX/DirectXManager.h>
#include <AssetManager/AssetManager.h>
#include <DrawSystem/DrawSystem.h>
#include <Camera/CameraManager.h>


EngineEditor::EngineEditor(
	WindowManager* windowManager,
	DirectXManager* dxManager,
	DrawSystem* drawSystem, 
	IOManager* ioManager, 
	CameraManager* cameraManager, 
	AssetManager* assetManager,
	FixFPS* fixFPS)
{
	modelEditor_ = std::make_unique<ModelPreview>(dxManager, cameraManager, assetManager->GetModelManager()->GetModelBank());
	textureEditor_ = std::make_unique<TexturePreview>(dxManager, assetManager->GetTextureManager()->GetTextureBank());

	renderTexturePreview_ = std::make_unique<RenderTexturePreview>(dxManager);

	timeEditor_ = std::make_unique<TimeEditor>(fixFPS, dxManager->GetFrameProfiler());
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
	renderTexturePreview_->DrawImGui();
	timeEditor_->DrawImGui();
}

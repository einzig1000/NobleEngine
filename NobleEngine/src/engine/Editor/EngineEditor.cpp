#include "EngineEditor.h"
#include "ModelPreview/ModelPreview.h"
#include "TexturePreview/TexturePreview.h"
#include "RenderTexturePreview/RenderTexturePreview.h"

#include <IO/IOManager.h>
#include <Window/WindowManager.h>
#include <DirectX/DirectXManager.h>
#include <ResourceManager/ResourceManager.h>
#include <DrawSystem/DrawSystem.h>
#include <Camera/CameraManager.h>


EngineEditor::EngineEditor(
	WindowManager* windowManager,
	DirectXManager* dxManager,
	DrawSystem* drawSystem, 
	IOManager* ioManager, 
	CameraManager* cameraManager, 
	ResourceManager* resourceManager)
	:
	windowManager_(windowManager), 
	dxManager_(dxManager),
	drawSystem_(drawSystem), 
	ioManager_(ioManager), 
	cameraManager_(cameraManager),
	resourceManager_(resourceManager)
{
	modelEditor_ = std::make_unique<ModelPreview>(dxManager, cameraManager, resourceManager->GetModelManager()->GetModelBank());
	textureEditor_ = std::make_unique<TexturePreview>(dxManager, resourceManager->GetTextureManager()->GetTextureBank());

	renderTexturePreview_ = std::make_unique<RenderTexturePreview>(dxManager);
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
}

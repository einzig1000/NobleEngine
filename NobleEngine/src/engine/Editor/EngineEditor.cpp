#include "EngineEditor.h"
#include "ModelEditor/ModelEditor.h"
#include "TextureEditor/TextureEditor.h"

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
	modelEditor_ = std::make_unique<ModelEditor>(dxManager, cameraManager, resourceManager->GetModelManager()->GetModelBank());
	textureEditor_ = std::make_unique<TextureEditor>(dxManager, resourceManager->GetTextureManager()->GetTextureBank());
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
}

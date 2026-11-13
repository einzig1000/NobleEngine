#include "AssetsEditor.h"
#include <Editor/AssetsEditor/ModelPreview/ModelPreview.h>
#include <Editor/AssetsEditor/TexturePreview/TexturePreview.h>
#include <Editor/AssetsEditor/AudioPreview/AudioPreview.h>
#include <Editor/AssetsEditor/RenderTexturePreview/RenderTexturePreview.h>

#include <AssetManager/AssetManager.h>
#include <ImGuiManager/ImGuiManager.h>

AssetsEditor::AssetsEditor(DirectXManager* dxManager, CameraManager* cameraManager, AssetManager* assetManager)
{
	// 作るときだけ具体的な型を使い、あとは IEditorPanel として扱う
	tabs_.push_back(std::make_unique<ModelPreview>(dxManager, cameraManager, assetManager->GetModelManager()->GetModelBank()));
	tabs_.push_back(std::make_unique<TexturePreview>(dxManager, assetManager->GetTextureManager()->GetTextureBank()));
	tabs_.push_back(std::make_unique<AudioPreview>(assetManager->GetAudioManager()->GetAudioBank(), assetManager->GetAudioManager()->GetAudioPlayer()));
	tabs_.push_back(std::make_unique<RenderTexturePreview>(dxManager));
}

AssetsEditor::~AssetsEditor()
{}

void AssetsEditor::Update()
{
	for (auto& tab : tabs_)
	{
		tab->Update();
	}
}

void AssetsEditor::Draw()
{
	for (auto& tab : tabs_)
	{
		tab->Draw();
	}
}

void AssetsEditor::DrawImGui()
{
	ImGui::Begin("Assets Editor");
	ImGui::BeginTabBar("AssetsEditor");
	for (auto& tab : tabs_)
	{
		tab->DrawImGui();
	}
	ImGui::EndTabBar();
	ImGui::End();
}
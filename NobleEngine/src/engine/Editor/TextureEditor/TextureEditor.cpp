#include "TextureEditor.h"
#include <ImGuiManager/ImGuiManager.h>
#include <ResourceManager/Texture/TextureBank/TextureBank.h>
#include <DirectX/DirectXManager.h>

TextureEditor::TextureEditor(DirectXManager* dxManager, TextureBank* bank)
	: dxManager_(dxManager), bank_(bank)
{}

TextureEditor::~TextureEditor()
{}

void TextureEditor::Update()
{}

void TextureEditor::Draw()
{}

void TextureEditor::DrawImGui()
{
	ImGui::Begin("Texture Editor");

	if (ImGui::BeginListBox("##texture list"))
	{
		std::unordered_map<int32_t, std::unique_ptr<TextureData>>& textureList = bank_->GetTextureList();

		for (auto& texture : textureList)
		{
			ImGui::BeginGroup();
			const std::string& filePath = texture.second->filePath;
			if (ImGui::Selectable(filePath.c_str(), textureID_ == texture.first))
			{
				textureID_ = texture.first;
			}
			ImGui::EndGroup();
		}
		ImGui::EndListBox();
	}
	ImGui::SameLine();

	if (ImGui::ImageButton("##sssed", ImTextureID(dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->GetGPUHandleAt(textureID_).ptr), ImVec2(128, 128)))
	{
		fullscreen_ = !fullscreen_;
	}

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ImGui::SetDragDropPayload("DAD_TEXTURE_ID", &textureID_, sizeof(int32_t));
		ImGui::Text("Texture ID %d", textureID_);
		ImGui::EndDragDropSource();
	}

	if (fullscreen_)
	{
		ImGui::Begin("Texture Preview", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		ImGui::Image(ImTextureID(dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->GetGPUHandleAt(textureID_).ptr), ImVec2(512, 512));
		
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload("DAD_TEXTURE_ID", &textureID_, sizeof(int32_t));
			ImGui::Text("Texture ID %d", textureID_);
			ImGui::EndDragDropSource();
		}

		ImGui::End();
	}


	ImGui::End();
}

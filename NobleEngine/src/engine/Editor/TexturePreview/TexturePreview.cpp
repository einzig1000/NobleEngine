#include "TexturePreview.h"
#include <ImGuiManager/ImGuiManager.h>
#include <AssetManager/Texture/TextureBank/TextureBank.h>
#include <DirectX/DirectXManager.h>

TexturePreview::TexturePreview(DirectXManager* dxManager, TextureBank* bank)
	: dxManager_(dxManager), bank_(bank)
{}

TexturePreview::~TexturePreview()
{}

void TexturePreview::Update()
{}

void TexturePreview::Draw()
{}

void TexturePreview::DrawImGui()
{
	ImGui::Begin("Texture Preview");
	const std::unordered_map<int32_t, std::unique_ptr<TextureData>>& textureList = bank_->GetTextureMap();

	if (ImGui::BeginListBox("##texture list"))
	{
		for (auto& texture : textureList)
		{
			ImGui::BeginGroup();
			const std::string& filePath = texture.second->filePath;
			if (ImGui::Selectable(filePath.c_str(), textureID_ == texture.first))
			{
				textureID_ = texture.first;
				size_t textureW = bank_->GetTextureMap().at(textureID_)->metadata.width;
				size_t textureH = bank_->GetTextureMap().at(textureID_)->metadata.height;

				// 長い方を512に合わせて正規化
				bool isWidthLonger = textureW > textureH;
				if (isWidthLonger)
				{
					windowSize_.x = 512;
					windowSize_.y = static_cast<int32_t>(512.0f * (static_cast<float>(textureH) / static_cast<float>(textureW)));
				}
				else
				{
					windowSize_.y = 512;
					windowSize_.x = static_cast<int32_t>(512.0f * (static_cast<float>(textureW) / static_cast<float>(textureH)));
				}
			}
			ImGui::EndGroup();
		}
		ImGui::EndListBox();
	}
	ImGui::SameLine();

	if (textureID_ != -1 && ImGui::ImageButton("##sssed", ImTextureID(dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->GetGPUHandleAt(textureID_).ptr), ImVec2(128, 128)))
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
		ImGui::Begin("texturepreview");

		ImGui::Image(ImTextureID(dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->GetGPUHandleAt(textureID_).ptr), ImVec2(static_cast<float>(windowSize_.x), static_cast<float>(windowSize_.y)));

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

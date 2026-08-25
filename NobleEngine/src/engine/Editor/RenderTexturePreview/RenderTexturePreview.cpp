#include "RenderTexturePreview.h"
#include <ImGuiManager/ImGuiManager.h>
#include <DirectX/DirectXManager.h>
#include <DirectX/RenderTarget/RenderTextureManager/RenderTextureManager.h>

RenderTexturePreview::RenderTexturePreview(DirectXManager* dxManager)
	: dxManager_(dxManager)
{}

RenderTexturePreview::~RenderTexturePreview()
{}

void RenderTexturePreview::Update()
{}

void RenderTexturePreview::Draw()
{}

void RenderTexturePreview::DrawImGui()
{
	ImGui::Begin("RenderTexture Preview");

	std::vector<std::unique_ptr<RenderTarget>>& renderTargets = dxManager_->GetRenderTextureManager()->GetRenderTargets();

	if (ImGui::BeginListBox("##renderTexture list"))
	{
		for (int32_t i = 0; i < renderTargets.size(); ++i)
		{
			ImGui::PushID(i);
			ImGui::BeginGroup();
			if (ImGui::Selectable(renderTargets[i]->name.c_str(), false, 0))
			{
				gpuDescriptorHandlePtr_ = renderTargets[i]->colorsrvAlloc.gpu.ptr;
				renderTargetIndex_ = i;

				size_t textureW = size_t(renderTargets[renderTargetIndex_]->width);
				size_t textureH = size_t(renderTargets[renderTargetIndex_]->height);
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
			ImGui::PopID();
		}
		ImGui::EndListBox();
	}
	ImGui::SameLine();


	if (renderTargetIndex_ != 22193211 && ImGui::ImageButton("##sssed", ImTextureID(gpuDescriptorHandlePtr_), ImVec2(128, 128)))
	{
		fullscreen_ = !fullscreen_;
	}
	if (renderTargetIndex_ != 22193211 && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ImGui::SetDragDropPayload("DAD_TEXTURE_ID", &renderTargets[renderTargetIndex_]->colorsrvAlloc.index, sizeof(int32_t));
		ImGui::Text("Texture ID %d", renderTargets[renderTargetIndex_]->colorsrvAlloc.index);
		ImGui::EndDragDropSource();
	}

	if (fullscreen_)
	{
		ImGui::Begin("rendertexturepreview");

		ImGui::Image(ImTextureID(gpuDescriptorHandlePtr_), ImVec2(static_cast<float>(windowSize_.x), static_cast<float>(windowSize_.y)));

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload("DAD_TEXTURE_ID", &renderTargets[renderTargetIndex_]->colorsrvAlloc.index, sizeof(int32_t));
			ImGui::Text("Texture ID %d", renderTargets[renderTargetIndex_]->colorsrvAlloc.index);
			ImGui::EndDragDropSource();
		}
		ImGui::End();
	}


	ImGui::End();
}

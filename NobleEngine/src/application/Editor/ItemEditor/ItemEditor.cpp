#include "ItemEditor.h"
#include <ResourceLoader/Data/DataManager.h>
#include <App.h>

ItemEditor::ItemEditor(DataManager* dataManager)
{
	renderObject_ = std::make_unique<RenderObject>();
	renderObject_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	renderObject_->psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModel.VS.hlsl";
	renderObject_->modelID_ = 0;
	renderObject_->SetupFromShaders();

	renderTextureID_ = Game::Asset::RenderTexture::CreateRenderTexture(512, 512, "ItemEditorTexture");
	cameraID_ = Game::Camera::AddCamera("ItemEditorCamera");
	Game::Camera::Setter::SetScreenSize(Vector2(512, 512), 0, EaseType::IN_BACK, cameraID_);
}

ItemEditor::~ItemEditor()
{

}

void ItemEditor::Update()
{
	Game::Camera::Update(cameraID_);

	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(transforms_.scale, transforms_.rotate, transforms_.translate);
	Matrix4x4 wpv = world * Game::Camera::Getter::GetViewProjectionMatrix(cameraID_);
	renderObject_->SetCBufferData(0, ShaderType::PixelShader, &color);
	renderObject_->SetCBufferData(1, ShaderType::PixelShader, &textureID_);
	renderObject_->SetCBufferData(0, ShaderType::VertexShader, &wpv);
	renderObject_->SetCBufferData(1, ShaderType::VertexShader, &world);
}

void ItemEditor::Draw()
{
	if (renderObject_->modelID_ > 0 && textureID_ > 0)
	{
		renderObject_->Draw(renderTextureID_);
	}
}

void ItemEditor::DrawImGui()
{
	ImGui::Begin("Item Editor");

	Matrix4x4 viewMatrix = Game::Camera::Getter::GetViewMatrix(cameraID_);
	Matrix4x4 projectionMatrix = Game::Camera::Getter::GetProjectionMatrix(cameraID_);
	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transforms_.scale, transforms_.rotate, transforms_.translate);

	// ミニプレビュー画面 兼 フルスクボタン
	if (ImGui::ImageButton("##ss", ImTextureID(Game::Asset::RenderTexture::GetRenderTextureGPUPtr(renderTextureID_)), ImVec2(128, 128)))
	{
		fullscreen_ = !fullscreen_;
	}

	// テクスチャ・モデルの選択
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_TEXTURE_ID"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int32_t));
			textureID_ = *reinterpret_cast<const int32_t*>(payload->Data);
		}
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_MODEL_ID"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int32_t));
			renderObject_->modelID_ = *reinterpret_cast<const int32_t*>(payload->Data);
		}
		ImGui::EndDragDropTarget();
	}

	// 保存
	if (ImGui::Button("Save"))
	{	
		//App::Data::Item::Save(ItemID::Armor_Body_of_Diamond)
	}


	// フルスクリーン表示
	if (fullscreen_)
	{
		ImGui::Begin("Item Preview");

		ImGui::Image(ImTextureID(Game::Asset::RenderTexture::GetRenderTextureGPUPtr(renderTextureID_)), ImVec2(512, 512));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_TEXTURE_ID"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int32_t));
				textureID_ = *reinterpret_cast<const int32_t*>(payload->Data);
			}
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_MODEL_ID"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int32_t));
				renderObject_->modelID_ = *reinterpret_cast<const int32_t*>(payload->Data);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
	}

	ImGui::End();
}

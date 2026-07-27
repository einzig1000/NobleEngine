#include "ItemEditor.h"
#include <ResourceLoader/Data/DataManager.h>
#include <App.h>

ItemEditor::ItemEditor(DataManager* dataManager)
{
	renderObject_ = std::make_unique<RenderObject>();
	renderObject_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	renderObject_->psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModel.VS.hlsl";
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

	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(transforms_.scale, transforms_.rotate, transforms_.translate);
	Matrix4x4 wpv = world * Game::Camera::Getter::GetViewProjectionMatrix(cameraID_);
	renderObject_->SetCBufferData(0, ShaderType::PixelShader, &color_);
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

	// アイテムジャンル
	const char* items[] = { "None", "Armor", "Tool", "Block", "Object" };
	static ItemGenre current_item = ItemGenre::None;
	if (ImGui::BeginCombo("ItemGenre", items[static_cast<size_t>(current_item)]))
	{
		for (size_t i = 0; i < IM_ARRAYSIZE(items); i++)
		{
			bool is_selected = (current_item == static_cast<ItemGenre>(i));
			if (ImGui::Selectable(items[i], is_selected))
			{
				current_item = static_cast<ItemGenre>(i);
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// 現在の情報描画

	switch (current_item)
	{
	case ItemGenre::None:
	{
		break;
	}
	case ItemGenre::Armor:
	{
		ModelData* modelData = Game::Asset::Model::GetData(renderObject_->modelID_);
		std::string modelName = modelData ? modelData->filePath : "None";
		ImGui::Text("ModelPath	: %s", modelName.c_str());

		TextureData* textureData = Game::Asset::Texture::GetData(textureID_);
		std::string textureName = textureData ? textureData->filePath : "None";
		ImGui::Text("TexturePath: %s", textureName.c_str());

		break;
	}
	case ItemGenre::Tool:
	{
		ModelData* modelData = Game::Asset::Model::GetData(renderObject_->modelID_);
		std::string modelName = modelData ? modelData->filePath : "None";
		ImGui::Text("ModelPath	: %s", modelName.c_str());

		TextureData* textureData = Game::Asset::Texture::GetData(textureID_);
		std::string textureName = textureData ? textureData->filePath : "None";
		ImGui::Text("TexturePath: %s", textureName.c_str());

		break;
	}
	case ItemGenre::Block:
	{
		ImGui::ColorEdit4("Color", &color_.x);


		break;
	}
	case ItemGenre::Object:
	{
		ModelData* modelData = Game::Asset::Model::GetData(renderObject_->modelID_);
		std::string modelName = modelData ? modelData->filePath : "None";
		ImGui::Text("ModelPath	: %s", modelName.c_str());

		TextureData* textureData = Game::Asset::Texture::GetData(textureID_);
		std::string textureName = textureData ? textureData->filePath : "None";
		ImGui::Text("TexturePath: %s", textureName.c_str());

		break;
	}
	default:
		break;
	}

	// 保存
	if (ImGui::Button("Save"))
	{	
		//App::Data::Item::Save()
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

#include "ModelEditor.h"
#include <Camera/CameraManager.h>
#include <Camera/Camera.h>
#include <DirectX/DirectXManager.h>
#include <ImGuiManager/ImGuiManager.h>
#include <ResourceManager/Model/ModelBank/ModelBank.h>

ModelEditor::ModelEditor(DirectXManager* dxManager, CameraManager* cameraManager, ModelBank* bank)
	: dxManager_(dxManager), cameraManager_(cameraManager), bank_(bank)
{
	modelRenderObject_ = std::make_unique<RenderObject>();
	modelRenderObject_->psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModel.VS.hlsl";
	modelRenderObject_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	modelRenderObject_->SetupFromShaders();

	Vector2 renderTargetSize = Vector2(512, 512);

	cameraID_ = cameraManager_->AddCamera();
	cameraManager_->GetCamera(cameraID_)->SetScreenSizeTarget(renderTargetSize, 0, EaseType::IN_BACK);

	renderTarget_ = dxManager_->GetRenderTextureManager()->CreateRenderTarget(
		UINT(renderTargetSize.x),
		UINT(renderTargetSize.y),
		DXGI_FORMAT_R8G8B8A8_UNORM,
		"ModelPreviewRenderTarget",
		1.0f
	);

	textureID = 1;
}

ModelEditor::~ModelEditor()
{}

void ModelEditor::Update()
{
	cameraManager_->GetCamera(cameraID_)->Update();
	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(objectTransform_.scale, objectTransform_.rotate, objectTransform_.translate);
	Matrix4x4 wpv = world * cameraManager_->GetCamera(cameraID_)->GetViewProjectionMatrix();
	modelRenderObject_->SetCBufferData(0, ShaderType::PixelShader, &color);
	modelRenderObject_->SetCBufferData(1, ShaderType::PixelShader, &textureID);
	modelRenderObject_->SetCBufferData(0, ShaderType::VertexShader, &wpv);
	modelRenderObject_->SetCBufferData(1, ShaderType::VertexShader, &world);
}

void ModelEditor::Draw()
{
	modelRenderObject_->Draw(renderTarget_);
}

void ModelEditor::DrawImGui()
{
	ImGui::Begin("Model Editor");

	if (ImGui::BeginListBox("##model list"))
	{
		for (int i = 0; i < (int)bank_->GetModelList().size(); ++i)
		{
			ImGui::PushID(i);
			ImGui::BeginGroup();
			if (ImGui::Selectable(bank_->GetModelList()[i]->filePath.c_str(), false, 0))
			{
				modelRenderObject_->modelID_ = i;
			}
			ImGui::EndGroup();
			ImGui::PopID();
		}
		ImGui::EndListBox();
	}
	ImGui::SameLine();

	if (ImGui::ImageButton("##ss", ImTextureID(dxManager_->GetRenderTextureManager()->Get(renderTarget_)->colorsrvAlloc.gpu.ptr), ImVec2(128, 128)))
	{
		fullscreen_ = !fullscreen_;
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_TEXTURE_ID"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int32_t));
			textureID = *(const int32_t*)payload->Data;
		}
		ImGui::EndDragDropTarget();
	}

	if (fullscreen_)
	{
		ImGui::Begin("Model Preview", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize );

		ImGui::Image(ImTextureID(dxManager_->GetRenderTextureManager()->Get(renderTarget_)->colorsrvAlloc.gpu.ptr), ImVec2(512, 512));
		
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_TEXTURE_ID"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int32_t));
				textureID = *(const int32_t*)payload->Data;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
	}


	ImGui::End();
}

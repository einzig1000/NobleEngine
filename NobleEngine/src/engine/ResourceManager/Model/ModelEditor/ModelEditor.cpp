#include "ModelEditor.h"
#include <Engine.h>
#include <Camera/CameraManager.h>
#include <Camera/Camera.h>
#include <DirectX/DirectXManager.h>
#include <ImGuiManager/ImGuiManager.h>
#include <ResourceManager/Model/ModelBank/ModelBank.h>


ModelEditor::ModelEditor(DirectXManager* dxManager, ModelBank* bank)
	: dxManager_(dxManager), bank_(bank)
{
	modelRenderObject_ = std::make_unique<RenderObject>();
	modelRenderObject_->psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModel.VS.hlsl";
	modelRenderObject_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	modelRenderObject_->SetupFromShaders();

	renderTargetSize_ = Vector2(128, 128);

	cameraID_ = Engine::Instance().GetCameraManager()->AddCamera();
	renderTarget_ = dxManager_->GetRenderTextureManager()->CreateRenderTarget(
		UINT(renderTargetSize_.x),
		UINT(renderTargetSize_.y),
		DXGI_FORMAT_R8G8B8A8_UNORM,
		"ModelPreviewRenderTarget"
	);
}

ModelEditor::~ModelEditor()
{}

void ModelEditor::Draw()
{
	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(objectTransform_.scale, objectTransform_.rotate, objectTransform_.translate);
	Matrix4x4 wpv = world * Engine::Instance().GetCameraManager()->GetCamera(cameraID_)->GetViewProjectionMatrix();
	int32_t textureID = 0;
	modelRenderObject_->SetCBufferData(0, ShaderType::PixelShader, &color);
	modelRenderObject_->SetCBufferData(1, ShaderType::PixelShader, &textureID);
	modelRenderObject_->SetCBufferData(0, ShaderType::VertexShader, &wpv);
	modelRenderObject_->SetCBufferData(1, ShaderType::VertexShader, &world);

	modelRenderObject_->Draw(renderTarget_);
}

void ModelEditor::DrawImGui()
{
	Engine::Instance().GetCameraManager()->Update(cameraID_);

	ImGui::Begin("Model Editor");

	if (ImGui::TreeNode("モデルリスト"))
	{
		if (ImGui::BeginListBox("##sihpo;dj", ImVec2(-FLT_MIN - 100, 200)))
		{
			for (int i = 0; i < (int)bank_->GetModelList().size(); ++i)
			{
				ImGui::PushID(i);
				ImGui::BeginGroup();
				if (ImGui::Selectable(bank_->GetModelList()[i]->filePath.c_str(), false, 0, ImVec2(200, 0)))
				{
					modelRenderObject_->modelID_ = i;
				}
				ImGui::EndGroup();
				ImGui::PopID();
			}
			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}

	ImGui::Image(ImTextureID(dxManager_->GetRenderTextureManager()->Get(renderTarget_)->colorsrvAlloc.gpu.ptr), ImVec2(128, 128));

	ImGui::End();
}

#include "ModelPreview.h"
#include <Camera/CameraManager.h>
#include <Camera/Camera.h>
#include <DirectX/DirectXManager.h>
#include <ImGuiManager/ImGuiManager.h>
#include <ResourceManager/Model/ModelBank/ModelBank.h>

ModelPreview::ModelPreview(DirectXManager* dxManager, CameraManager* cameraManager, ModelBank* bank)
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
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		"ModelPreviewRenderTarget" ,1.0f
	);

	textureID = 1;
}

ModelPreview::~ModelPreview()
{}

void ModelPreview::Update()
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

void ModelPreview::Draw()
{
	modelRenderObject_->Draw(renderTarget_);
}

void ModelPreview::DrawImGui()
{
	static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE; // 移動モード
	static ImGuizmo::MODE currentMode = ImGuizmo::WORLD;               // ワールド座標系


	Matrix4x4 viewMatrix = cameraManager_->GetCamera(cameraID_)->GetViewMatrix();
	Matrix4x4 projectionMatrix = cameraManager_->GetCamera(cameraID_)->GetProjectionMatrix();
	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(objectTransform_.scale, objectTransform_.rotate, objectTransform_.translate);

	DirectX::XMMATRIX viewXM = DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&viewMatrix));
	DirectX::XMMATRIX projectionXM = DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&projectionMatrix));
	DirectX::XMMATRIX worldXM = DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&worldMatrix));

	DirectX::XMFLOAT4X4 viewF, projF, worldF;
	DirectX::XMStoreFloat4x4(&viewF, viewXM);
	DirectX::XMStoreFloat4x4(&projF, projectionXM);
	DirectX::XMStoreFloat4x4(&worldF, worldXM);


	// モデル選択、ミニプレビュー表示
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

	if (modelRenderObject_->modelID_ != -1 && ImGui::ImageButton("##ss", ImTextureID(dxManager_->GetRenderTextureManager()->Get(renderTarget_)->colorsrvAlloc.gpu.ptr), ImVec2(128, 128)))
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

	ImGui::End();


	// フルスクリーン表示
	if (fullscreen_)
	{
		//ImGui::Begin("Model Preview", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
		ImGui::Begin("Model Preview");

		if (ImGui::RadioButton("Translate", currentOperation == ImGuizmo::TRANSLATE)) currentOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", currentOperation == ImGuizmo::ROTATE)) currentOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", currentOperation == ImGuizmo::SCALE)) currentOperation = ImGuizmo::SCALE;

		ImVec2 imagePos = ImGui::GetCursorScreenPos();
		ImVec2 imageSize = ImVec2(512, 512);

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

		ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
		
		ImGuizmo::SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);

		bool manipulated = ImGuizmo::Manipulate(
			reinterpret_cast<const float*>(&viewF),
			reinterpret_cast<const float*>(&projF),
			currentOperation,
			currentMode,
			reinterpret_cast<float*>(&worldF)
		);

		if (manipulated)
		{
			DirectX::XMMATRIX newWorldXM = DirectX::XMLoadFloat4x4(&worldF);

			DirectX::XMVECTOR scaleV, rotQuatV, transV;
			DirectX::XMMatrixDecompose(&scaleV, &rotQuatV, &transV, newWorldXM);

			DirectX::XMFLOAT3 newScale; DirectX::XMFLOAT4 newRotQuat; DirectX::XMFLOAT3 newTrans;
			DirectX::XMStoreFloat3(&newScale, scaleV);
			DirectX::XMStoreFloat4(&newRotQuat, rotQuatV);
			DirectX::XMStoreFloat3(&newTrans, transV);

			objectTransform_.scale = { newScale.x, newScale.y, newScale.z };
			objectTransform_.rotate = { newRotQuat.x, newRotQuat.y, newRotQuat.z, newRotQuat.w };
			objectTransform_.translate = { newTrans.x, newTrans.y, newTrans.z };
		}

		ImGui::End();
	}
}

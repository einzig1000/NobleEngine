#include "ModelPreview.h"
#include <Engine.h>
#include <AssetManager/AssetManager.h>
#include <Camera/CameraManager.h>
#include <Camera/Camera.h>
#include <DirectX/DirectXManager.h>
#include <ImGuiManager/ImGuiManager.h>
#include <AssetManager/Model/ModelBank/ModelBank.h>
#include <RootBinding/StructuredBufferManager/StructuredBufferManager.h>
#include <numbers>

ModelPreview::ModelPreview(DirectXManager* dxManager, CameraManager* cameraManager, ModelBank* bank)
	: dxManager_(dxManager), cameraManager_(cameraManager), bank_(bank)
{
	modelRenderObject_ = std::make_unique<RenderObject>();
	modelRenderObject_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	modelRenderObject_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	modelRenderObject_->SetupFromShaders();

	Vector2 renderTargetSize = Vector2(512, 512);

	cameraID_ = cameraManager_->AddCamera("ModelPreview");
	cameraManager_->GetCamera(cameraID_)->SetScreenSizeTarget(renderTargetSize, 0, EaseType::IN_BACK);

	renderTarget_ = dxManager_->GetRenderTextureManager()->CreateRenderTarget(
		UINT(renderTargetSize.x),
		UINT(renderTargetSize.y),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		"ModelPreviewRenderTarget" ,Vector4{ 0.11f, 0.11f, 0.11f, 1.0f }
	);

	textureID = Engine::Instance().GetAssetManager()->GetTextureManager()->GetTextureLoader()->LoadTexture("assets/engine/texture/uvChecker.png");

	colliderCubeModelID_ = Engine::Instance().GetAssetManager()->GetModelManager()->GetModelLoader()->LoadModel("assets/engine/model/cube/cube.obj");
	colliderSphereModelID_ = Engine::Instance().GetAssetManager()->GetModelManager()->GetModelLoader()->LoadModel("assets/engine/model/sphere/sphere.obj");
	colliderTextureID_ = Engine::Instance().GetAssetManager()->GetTextureManager()->GetTextureLoader()->LoadTexture("assets/engine/texture/white1x1.png");
}

ModelPreview::~ModelPreview()
{}

void ModelPreview::Update()
{
	if (requestRebuildColliderRenderObjects_)
	{
		RebuildColliderRenderObjects();
		requestRebuildColliderRenderObjects_ = false;
	}
	cameraManager_->GetCamera(cameraID_)->Update();
	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(objectTransform_.scale, objectTransform_.rotate, objectTransform_.translate);
	Matrix4x4 wpv = world * cameraManager_->GetCamera(cameraID_)->GetViewProjectionMatrix();
	modelRenderObject_->SetCBufferData(0, ShaderType::PixelShader, &color);
	modelRenderObject_->SetCBufferData(1, ShaderType::PixelShader, &textureID);
	modelRenderObject_->SetCBufferData(0, ShaderType::VertexShader, &wpv);
	modelRenderObject_->SetCBufferData(1, ShaderType::VertexShader, &world);


	Vector4 colliderColor = Vector4(0.2f, 1.0f, 0.4f, 1.0f);
	const Matrix4x4& viewProjection = cameraManager_->GetCamera(cameraID_)->GetViewProjectionMatrix();
	const size_t aabbCount = colliderShape_.aabbs.size();

	for (size_t i = 0; i < colliderRender_.size(); ++i)
	{
		Vector3 center;
		Vector3 halfExtent;
		if (i < aabbCount)
		{
			const AABB& aabb = colliderShape_.aabbs[i];
			center = aabb.center();
			halfExtent = (aabb.max - aabb.min) * 0.5f;
		}
		else
		{
			const Sphere& sphere = colliderShape_.spheres[i - aabbCount];
			center = sphere.center;
			halfExtent = Vector3(sphere.radius, sphere.radius, sphere.radius);
		}

		Matrix4x4 colliderLocal = Matrix4x4::MakeAffineMatrix(halfExtent, Vector3(0.0f, 0.0f, 0.0f), center);
		Matrix4x4 colliderWorld = colliderLocal * world;
		Matrix4x4 colliderWvp = colliderWorld * viewProjection;

		colliderRender_[i]->SetCBufferData(0, ShaderType::PixelShader, &colliderColor);
		colliderRender_[i]->SetCBufferData(1, ShaderType::PixelShader, &colliderTextureID_);
		colliderRender_[i]->SetCBufferData(0, ShaderType::VertexShader, &colliderWvp);
		colliderRender_[i]->SetCBufferData(1, ShaderType::VertexShader, &colliderWorld);
	}
}

void ModelPreview::Draw()
{
	modelRenderObject_->Draw(renderTarget_);

	for (const auto& collider : colliderRender_)
	{
		collider->Draw(renderTarget_);
	}
}

void ModelPreview::DrawImGui()
{
	// モデルリスト表示
	ImGui::Begin("Model Editor");

	if (ImGui::BeginListBox("##model list"))
	{
		for (int32_t i = 0; i < (int32_t)bank_->GetModelList().size(); ++i)
		{
			ImGui::PushID(i);
			ImGui::BeginGroup();
			if (ImGui::Selectable(bank_->GetModelList()[i]->filePath.c_str(), false, 0))
			{
				modelRenderObject_->modelID_ = i;
				modelData_ = bank_->GetModelData(i);
				requestRebuildColliderRenderObjects_ = true;
			}
			ImGui::EndGroup();
			ImGui::PopID();
		}
		ImGui::EndListBox();
	}
	ImGui::SameLine();

	// ミニプレビュー表示
	if (modelRenderObject_->modelID_ != -1 && ImGui::ImageButton("##ss", ImTextureID(dxManager_->GetRenderTextureManager()->Get(renderTarget_)->colorsrvAlloc.gpu.ptr), ImVec2(128, 128)))
	{
		fullscreen_ = !fullscreen_;
	}
	// DAD処理
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_TEXTURE_ID"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int32_t));
			textureID = *reinterpret_cast<const int32_t*>(payload->Data);
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ImGui::SetDragDropPayload("DAD_MODEL_ID", &modelRenderObject_->modelID_, sizeof(int32_t));
		ImGui::Text("Model ID %d", modelRenderObject_->modelID_);
		ImGui::EndDragDropSource();
	}

	ImGui::End();


	// フルスクリーン表示
	if (fullscreen_)
	{
		ImGui::Begin("Model Preview");

		// でか画像表示
		ImVec2 imTextureSize = ImVec2{ 448.0f, 448.0f };
		ImVec2 imagePos = ImGui::GetCursorScreenPos();
		ImGui::Image(ImTextureID(dxManager_->GetRenderTextureManager()->Get(renderTarget_)->colorsrvAlloc.gpu.ptr), imTextureSize);
		ImGui::SameLine();
		ImGui::Checkbox("Edit Collider", &isEditingCollider_);

		// コライダー＋ギズモ表示
		if (isEditingCollider_)
		{

			Matrix4x4 viewMatrix = cameraManager_->GetCamera(cameraID_)->GetViewMatrix();
			Matrix4x4 projectionMatrix = cameraManager_->GetCamera(cameraID_)->GetProjectionMatrix();
			Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(objectTransform_.scale, objectTransform_.rotate, objectTransform_.translate);

			float viewM[16], projM[16], worldM[16];
			std::memcpy(viewM, viewMatrix.m, sizeof(float) * 16);
			std::memcpy(projM, projectionMatrix.m, sizeof(float) * 16);
			std::memcpy(worldM, worldMatrix.m, sizeof(float) * 16);

			static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE; // 移動モード
			static ImGuizmo::MODE currentMode = ImGuizmo::WORLD;   


			// ギズモボタン
			ImGui::SetCursorPosX(imTextureSize.x + 15.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - imTextureSize.y + 20.0f);
			if (ImGui::RadioButton("Translate", currentOperation == ImGuizmo::TRANSLATE)) currentOperation = ImGuizmo::TRANSLATE;
			ImGui::SameLine();
			if (ImGui::RadioButton("Rotate", currentOperation == ImGuizmo::ROTATE)) currentOperation = ImGuizmo::ROTATE;
			ImGui::SameLine();
			if (ImGui::RadioButton("Scale", currentOperation == ImGuizmo::SCALE)) currentOperation = ImGuizmo::SCALE;

			// コライダーリスト
			ImGui::SetCursorPosX(imTextureSize.x + 15.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
			if (ImGui::BeginListBox("##modelCollider list"))
			{
				for (size_t i = 0; i < colliderShape_.aabbs.size(); ++i)
				{
					ImGui::PushID(static_cast<int>(i));
					ImGui::BeginGroup();
					std::string label = "AABB " + std::to_string(i);
					if (ImGui::Selectable(label.c_str(), false, 0))
					{
						//modelRenderObject_->modelID_ = i;
					}
					ImGui::EndGroup();
					ImGui::PopID();
				}
				for (size_t i = 0; i < colliderShape_.spheres.size(); ++i)
				{
					ImGui::PushID(static_cast<int>(i + colliderShape_.aabbs.size()));
					ImGui::BeginGroup();
					std::string label = "Sphere " + std::to_string(i);
					if (ImGui::Selectable(label.c_str(), false, 0))
					{
						//modelRenderObject_->modelID_ = i + colliderShape_.aabbs.size();
					}
					ImGui::EndGroup();
					ImGui::PopID();
				}

				ImGui::EndListBox();
			}


			// ギズモ描画
			ImGui::SetCursorScreenPos(imagePos);
			ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
			ImGuizmo::SetRect(imagePos.x, imagePos.y, imTextureSize.x, imTextureSize.y);

			bool manipulated = ImGuizmo::Manipulate(
				viewM,
				projM,
				currentOperation,
				currentMode,
				worldM
			);

			if (manipulated)
			{
				float imScale[3], imRotate[3], imTranslate[3];
				ImGuizmo::DecomposeMatrixToComponents(worldM, imTranslate, imRotate, imScale);
				imRotate[0] *= std::numbers::pi_v<float> / 180.0f;
				imRotate[1] *= std::numbers::pi_v<float> / 180.0f;
				imRotate[2] *= std::numbers::pi_v<float> / 180.0f;

				objectTransform_.scale = { imScale[0], imScale[1], imScale[2] };
				objectTransform_.rotate = { imRotate[0], imRotate[1], imRotate[2] };
				objectTransform_.translate = { imTranslate[0], imTranslate[1], imTranslate[2] };
			}
		}

		// DAD処理
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload("DAD_MODEL_ID", &modelRenderObject_->modelID_, sizeof(int32_t));
			ImGui::Text("Model ID %d", modelRenderObject_->modelID_);
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_TEXTURE_ID"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int32_t));
				textureID = *reinterpret_cast<const int32_t*>(payload->Data);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
	}
}

void ModelPreview::RebuildColliderRenderObjects()
{
	colliderShape_ = modelData_->colliderShape;
	const size_t aabbCount = colliderShape_.aabbs.size();
	const size_t sphereCount = colliderShape_.spheres.size();

	colliderRender_.clear();
	colliderRender_.resize(aabbCount + sphereCount);

	for (size_t i = 0; i < colliderRender_.size(); ++i)
	{
		colliderRender_[i] = std::make_unique<RenderObject>();
		colliderRender_[i]->modelID_ = (i < aabbCount) ? colliderCubeModelID_ : colliderSphereModelID_;
		colliderRender_[i]->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
		colliderRender_[i]->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
		colliderRender_[i]->psoConfig_.rasterizerID = RasterizerID::Wireframe_NoCull;
		colliderRender_[i]->SetupFromShaders();
	}
}

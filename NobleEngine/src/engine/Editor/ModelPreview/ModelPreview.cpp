#include "ModelPreview.h"
#include <Engine.h>
#include <AssetManager/AssetManager.h>
#include <Camera/CameraManager.h>
#include <Camera/Camera.h>
#include <DirectX/DirectXManager.h>
#include <ImGuiManager/ImGuiManager.h>
#include <AssetManager/Model/ModelBank/ModelBank.h>
#include <AssetManager/Model/ModelHelper/ModelHelper.h>
#include <numbers>
#include <Utilities/FileDialog/FileDialog.h>
#include <Window/WindowManager.h>
#include <filesystem>



ModelPreview::ModelPreview(DirectXManager* dxManager, CameraManager* cameraManager, ModelBank* bank)
	: dxManager_(dxManager), cameraManager_(cameraManager), bank_(bank)
{
	modelRenderObject_ = std::make_unique<RenderObject>();
	modelRenderObject_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	modelRenderObject_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	modelRenderObject_->psoConfig_.depthStencilID = DepthStencilID::TestOnly;
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
	// 前フレームのImGuiで更新されたデータの更新
	if (requestRebuildColliderRenderObjects_)
	{
		RebuildColliderRenderObjects();
		requestRebuildColliderRenderObjects_ = false;
	}

	// カメラ更新
	cameraManager_->GetCamera(cameraID_)->Update();
	Matrix4x4 viewProjection = cameraManager_->GetCamera(cameraID_)->GetViewProjectionMatrix();

	// モデル描画
	color_ = isEditingCollider_ ? Vector4(1.0f, 1.0f, 1.0f, 0.5f) : Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	world_ = Matrix4x4::MakeAffineMatrix(objectTransform_.scale, objectTransform_.rotate, objectTransform_.translate);
	wpv_ = world_ * viewProjection;
	modelRenderObject_->psoConfig_.depthStencilID = isEditingCollider_ ? DepthStencilID::TestOnly : DepthStencilID::Default;


	if (isEditingCollider_)
	{
		const size_t aabbCount = colliderShape_.aabbs.size();
		for (size_t i = 0; i < colliderRender_.size(); ++i)
		{
			Vector3 center;
			Vector3 halfExtent;
			if (i < aabbCount)
			{
				const AABB& aabb = colliderShape_.aabbs[i];
				center = aabb.center();
				halfExtent = (aabb.max - aabb.min);
			}
			else
			{
				const Sphere& sphere = colliderShape_.spheres[i - aabbCount];
				center = sphere.center;
				halfExtent = Vector3(sphere.radius, sphere.radius, sphere.radius);
			}

			Matrix4x4 colliderLocal = Matrix4x4::MakeAffineMatrix(halfExtent, Vector3(0.0f, 0.0f, 0.0f), center);
			colliderWorld_[i] = colliderLocal * world_;
			colliderWpv_[i] = colliderWorld_[i] * viewProjection;
			colliderColor_[i] = (i == selectedColliderIndex_) ? Vector4(1.0f, 0.0f, 0.0f, 1.0f) : Vector4(0.2f, 1.0f, 0.4f, 1.0f);
		}
	}
}

void ModelPreview::Draw()
{
	modelRenderObject_->SetBRegisterData(0, ShaderType::PixelShader, &color_);
	modelRenderObject_->SetBRegisterData(1, ShaderType::PixelShader, &textureID);
	modelRenderObject_->SetBRegisterData(0, ShaderType::VertexShader, &wpv_);
	modelRenderObject_->SetBRegisterData(1, ShaderType::VertexShader, &world_);
	modelRenderObject_->Draw(renderTarget_);

	if (isEditingCollider_)
	{
		// コライダー描画
		for (size_t i = 0; i < colliderRender_.size(); ++i)
		{
			colliderRender_[i]->SetBRegisterData(0, ShaderType::PixelShader, &colliderColor_[i]);
			colliderRender_[i]->SetBRegisterData(1, ShaderType::PixelShader, &colliderTextureID_);
			colliderRender_[i]->SetBRegisterData(0, ShaderType::VertexShader, &colliderWpv_[i]);
			colliderRender_[i]->SetBRegisterData(1, ShaderType::VertexShader, &colliderWorld_[i]);
			colliderRender_[i]->Draw(renderTarget_);
		}
	}
}

void ModelPreview::DrawImGui()
{
	// モデルリスト表示
	ImGui::Begin("Model Editor");

	if (ImGui::Button("Open Model"))
	{
		std::string path = FileDialog::OpenFile(
			Engine::Instance().GetWindowManager()->GetHwnd(),
			L"Open Model",
			{ { L"Model Files", L"*.obj;*.gltf;*.fbx" }, { L"All Files", L"*.*" } }
		);

		if (!path.empty())
		{
			int32_t newID = Engine::Instance().GetAssetManager()->GetModelManager()->GetModelLoader()->LoadModel(path);
			SelectModel(newID);
		}
	}

				const ImGuiStyle& style = ImGui::GetStyle();
				const float copyButtonWidth = ImGui::CalcTextSize("Copy").x + style.FramePadding.x * 2.0f;
				const float selectableWidth = std::max(ImGui::GetContentRegionAvail().x - copyButtonWidth - style.ItemSpacing.x, 1.0f);
	if (ImGui::BeginListBox("##model list"))
	{
		for (int32_t i = 0; i < (int32_t)bank_->GetModelList().size(); ++i)
		{
			ImGui::PushID(i);
			ImGui::BeginGroup();
			if (ImGui::SelectableWithCopy(bank_->GetModelList()[i]->filePath, modelRenderObject_->modelID_ == i))
			{
				SelectModel(i);
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
		ImGui::SameLine();

		// ギズモ操作
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
		if (ImGui::RadioButton("Translate", currentOperation == ImGuizmo::TRANSLATE)) currentOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", currentOperation == ImGuizmo::ROTATE)) currentOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", currentOperation == ImGuizmo::SCALE)) currentOperation = ImGuizmo::SCALE;

		// ギズモ描画
		ImGui::SetCursorScreenPos(imagePos);
		ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
		ImGuizmo::SetRect(imagePos.x, imagePos.y, imTextureSize.x, imTextureSize.y);
		if (ImGuizmo::Manipulate(viewM, projM, currentOperation, currentMode, worldM))
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


		ImGui::SetCursorPosX(imTextureSize.x + 15.0f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 25.0f);
		ImGui::Checkbox("Edit Collider", &isEditingCollider_);
		// コライダー表示
		if (isEditingCollider_)
		{
			// コライダーリスト
			ImGui::SetCursorPosX(imTextureSize.x + 15.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
			ImGui::SetNextItemWidth(200.0f);
			if (ImGui::BeginListBox("##modelCollider list"))
			{
				for (size_t i = 0; i < colliderShape_.aabbs.size(); ++i)
				{
					ImGui::PushID(static_cast<int>(i));
					ImGui::BeginGroup();
					std::string label = "AABB " + std::to_string(i);
					if (ImGui::Selectable(label.c_str(), false, 0))
					{
						selectedColliderIndex_ = static_cast<int32_t>(i);
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
						selectedColliderIndex_ = static_cast<int32_t>(i + colliderShape_.aabbs.size());
					}
					ImGui::EndGroup();
					ImGui::PopID();
				}

				ImGui::EndListBox();
			}

			// コライダー追加ボタン
			ImGui::SetCursorPosX(imTextureSize.x + 15.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
			if (ImGui::Button("Add AABB"))
			{
				colliderShape_.aabbs.push_back(AABB{ Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f) });
				selectedColliderIndex_ = static_cast<int32_t>(colliderShape_.aabbs.size() - 1);
				requestRebuildColliderRenderObjects_ = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Add Sphere"))
			{
				colliderShape_.spheres.push_back(Sphere{ Vector3(0.0f, 0.0f, 0.0f), 0.5f });
				selectedColliderIndex_ = static_cast<int32_t>(colliderShape_.aabbs.size() + colliderShape_.spheres.size() - 1);
				requestRebuildColliderRenderObjects_ = true;
			}


			// 選択中のコライダーの編集
			const size_t aabbCount = colliderShape_.aabbs.size();
			const size_t sphereCount = colliderShape_.spheres.size();
			if (selectedColliderIndex_ >= 0 && selectedColliderIndex_ < static_cast<int32_t>(aabbCount))
			{
				AABB& aabb = colliderShape_.aabbs[selectedColliderIndex_];
				ImGui::SetCursorPosX(imTextureSize.x + 15.0f);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
				ImGui::Text("AABB %d", selectedColliderIndex_);
				ImGui::SetCursorPosX(imTextureSize.x + 15.0f);
				ImGui::SetNextItemWidth(200.0f);
				ImGui::DragFloat3("Min", &aabb.min.x, 0.01f);
				ImGui::SetCursorPosX(imTextureSize.x + 15.0f);
				ImGui::SetNextItemWidth(200.0f);
				ImGui::DragFloat3("Max", &aabb.max.x, 0.01f);
				aabb.Fix();
			}
			else if (selectedColliderIndex_ >= static_cast<int32_t>(aabbCount) && selectedColliderIndex_ < static_cast<int32_t>(aabbCount + sphereCount))
			{
				const int32_t sphereLocalIndex = selectedColliderIndex_ - static_cast<int32_t>(aabbCount);
				Sphere& sphere = colliderShape_.spheres[sphereLocalIndex];
				ImGui::SetCursorPosX(imTextureSize.x + 15.0f);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
				ImGui::Text("Sphere %d", sphereLocalIndex);
				ImGui::SetCursorPosX(imTextureSize.x + 15.0f);
				ImGui::SetNextItemWidth(200.0f);
				ImGui::DragFloat3("Center", &sphere.center.x, 0.01f);
				ImGui::SetCursorPosX(imTextureSize.x + 15.0f);
				ImGui::SetNextItemWidth(200.0f);
				ImGui::DragFloat("Radius", &sphere.radius, 0.01f, 0.01f, 100.0f);
			}

			ImGui::SetCursorPosX(imTextureSize.x + 15.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
			if (ImGui::Button("Save"))
			{
				auto path = std::filesystem::path(modelData_->filePath);
				// ディレクトリ名
				std::string directory = path.parent_path().string();
				// 拡張子を除いたファイル名
				std::string stem = path.stem().string();
				// ColliderShape.csvのパス
				std::string csvFilePath = directory + "/" + stem + ".csv";

				ModelHelper::SaveColliderShapesToCSV(csvFilePath, colliderShape_);
			}
		}


		ImGui::End();
	}
}

void ModelPreview::SelectModel(int32_t modelID)
{
	modelRenderObject_->modelID_ = modelID;
	modelData_ = bank_->GetModelData(modelID);
	selectedColliderIndex_ = -1;
	colliderShape_ = modelData_->colliderShape;
	requestRebuildColliderRenderObjects_ = true;
}

void ModelPreview::RebuildColliderRenderObjects()
{
	const size_t aabbCount = colliderShape_.aabbs.size();
	const size_t sphereCount = colliderShape_.spheres.size();
	const size_t totalCount = aabbCount + sphereCount;

	colliderRender_.clear();
	colliderRender_.resize(totalCount);
	colliderWpv_.clear();
	colliderWpv_.resize(totalCount);
	colliderWorld_.clear();
	colliderWorld_.resize(totalCount);
	colliderColor_.clear();
	colliderColor_.resize(totalCount);

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

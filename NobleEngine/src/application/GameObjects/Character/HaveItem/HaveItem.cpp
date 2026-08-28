#include "HaveItem.h"
#include <App.h>
#include <Utilities/functions.h>
#include <numbers>

namespace
{
	std::vector<AABB> CreateAABB(const std::vector<AABB>& aabbs, const Matrix4x4& worldMatrix)
	{
		std::vector<AABB> result;

		for (const auto& localAABB : aabbs)
		{
			// ローカルAABBの8頂点
			Vector3 corners[8] = {
				{localAABB.min.x, localAABB.min.y, localAABB.min.z},
				{localAABB.max.x, localAABB.min.y, localAABB.min.z},
				{localAABB.min.x, localAABB.max.y, localAABB.min.z},
				{localAABB.max.x, localAABB.max.y, localAABB.min.z},
				{localAABB.min.x, localAABB.min.y, localAABB.max.z},
				{localAABB.max.x, localAABB.min.y, localAABB.max.z},
				{localAABB.min.x, localAABB.max.y, localAABB.max.z},
				{localAABB.max.x, localAABB.max.y, localAABB.max.z},
			};

			// 8頂点をワールド空間に変換
			Vector3 worldMin = Transform(corners[0], worldMatrix);
			Vector3 worldMax = worldMin;
			for (int32_t i = 1; i < 8; ++i)
			{
				Vector3 v = Transform(corners[i], worldMatrix);
				worldMin.x = std::min(worldMin.x, v.x);
				worldMin.y = std::min(worldMin.y, v.y);
				worldMin.z = std::min(worldMin.z, v.z);
				worldMax.x = std::max(worldMax.x, v.x);
				worldMax.y = std::max(worldMax.y, v.y);
				worldMax.z = std::max(worldMax.z, v.z);
			}
			result.push_back({ worldMin, worldMax });
		}
		return result;
	}

	std::vector<OBB> CreateOBB(const std::vector<AABB>& localAabbs, const Matrix4x4& worldMatrix)
	{
		std::vector<OBB> result;
		result.reserve(localAabbs.size());

		for (const auto& localAABB : localAabbs)
		{
			result.push_back(OBB::MakeFromAABB(localAABB, worldMatrix));
		}
		return result;
	}

	// scaleは一番でかい軸の値を使う
	std::vector<Sphere> CreateSphere(const std::vector<Sphere>& localSpheres, const Matrix4x4& worldMatrix)
	{
		std::vector<Sphere> result;
		result.reserve(localSpheres.size());
		for (const auto& localSphere : localSpheres)
		{
			Vector3 worldCenter = Transform(localSphere.center, worldMatrix);
			float worldRadius = localSphere.radius * std::max({ worldMatrix.m[0][0], worldMatrix.m[1][1], worldMatrix.m[2][2] });
			result.push_back({ worldCenter, worldRadius });
		}
		return result;
	}
}


HaveItem::HaveItem()
{
	render_ = std::make_unique<RenderObject>();
	render_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	render_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	render_->SetupFromShaders();

	itemTransform_.translate = { 0.65f, -0.620f, 0.0f };
	itemTransform_.scale = { 1.0f, 1.0f, 1.0f };
	//itemTransform_.rotate = { -1.49f, 0.0f, -2.35619f };
	itemTransform_.rotate = { -2.57f, 0.0f, -2.35619f };
}

HaveItem::~HaveItem()
{}

//void HaveItem::Update(int32_t cameraID)
//{
//	if (currentItemID_ != ItemID::MAX)
//	{
//		Vector3 cameraCenter = Game::Camera::Getter::GetCenter(cameraID);
//		Vector3 cameraPos = Game::Camera::Getter::GetTranslate(cameraID);
//		Vector3 cameraDir = cameraCenter - cameraPos;
//		cameraDir.y = 0.0f;
//		cameraDir.Normalize();
//		pivotTransform_.rotate.y = std::atan2(cameraDir.x, cameraDir.z);
//
//		if (Game::IO::Mouse::IsJustPressed(0))
//		{
//			pivotTransform_.rotate.z = Game::Math::Rand::RandFloat(-1.0f, 1.0f, 1);
//		}
//
//		if (Game::IO::Mouse::IsHeld(0))
//		{
//			pivotTransform_.rotate.x += 0.5f;
//			pivotTransform_.rotate.z += 0.01f;
//		}
//
//		if (Game::IO::Mouse::IsJustReleased(0))
//		{
//			pivotTransform_.rotate.x = 0.0f;
//		}
//
//		const ToolID toolID = App::Data::Item::Get(currentItemID_)->toolID;
//		const ToolInfo* toolConfig = App::Data::Item::Get(toolID);
//
//		render_->modelID_ = toolConfig->modelID;
//		const ColliderShape& colliderShape = Game::Asset::Model::GetData(toolConfig->modelID)->colliderShape;
//
//		Matrix4x4 itemWorld = Matrix4x4::MakeAffineMatrix(itemTransform_.scale, itemTransform_.rotate, itemTransform_.translate);
//		Matrix4x4 pivotWorld = Matrix4x4::MakeAffineMatrix(pivotTransform_.scale, pivotTransform_.rotate, pivotTransform_.translate);
//		itemWorld = itemWorld * pivotWorld * parentWorldMatrix_;
//		Matrix4x4 wvp = itemWorld * Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
//		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
//		int32_t textureID = toolConfig->textureID;
//
//		render_->SetCBufferData(0, ShaderType::VertexShader, &wvp);
//		render_->SetCBufferData(1, ShaderType::VertexShader, &itemWorld);
//		render_->SetCBufferData(0, ShaderType::PixelShader, &color);
//		render_->SetCBufferData(1, ShaderType::PixelShader, &textureID);
//
//		//itemAABB_ = CreateAABB(colliderShape.aabbs, itemWorld);
//		//itemOBB_ = CreateOBB(colliderShape.aabbs, itemWorld);
//		worldCollider_.spheres = CreateSphere(colliderShape.spheres, itemWorld);
//	}
//	else
//	{
//		render_->modelID_ = -1;
//	}
//}

void HaveItem::Update(int32_t cameraID)
{
	if (currentItemID_ != ItemID::MAX)
	{
		const ItemInfo* itemInfo = App::Data::Item::Get(currentItemID_);

		Vector3 cameraDir = Game::Camera::Getter::GetCameraDirection(cameraID);
		cameraDir.y = 0.0f;
		cameraDir.Normalize();
		pivotTransform_.rotate.y = std::atan2(cameraDir.x, cameraDir.z);

		Matrix4x4 itemWorld = Matrix4x4::MakeAffineMatrix(itemTransform_.scale, itemTransform_.rotate, itemTransform_.translate);
		Matrix4x4 pivotWorld = Matrix4x4::MakeAffineMatrix(pivotTransform_.scale, pivotTransform_.rotate, pivotTransform_.translate);
		itemWorld = itemWorld * pivotWorld * parentWorldMatrix_;
		//itemWorld = parentWorldMatrix_;
		Matrix4x4 wvp = itemWorld * Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		int32_t textureID = itemInfo->textureID;
		render_->modelID_ = itemInfo->modelID;

		render_->SetCBufferData(0, ShaderType::VertexShader, &wvp);
		render_->SetCBufferData(1, ShaderType::VertexShader, &itemWorld);
		render_->SetCBufferData(0, ShaderType::PixelShader, &color);
		render_->SetCBufferData(1, ShaderType::PixelShader, &textureID);
	}
	else
	{
		render_->modelID_ = -1;
	}
}

void HaveItem::Draw(int32_t renderTextureID)
{
	if (render_->modelID_ >= 0)
	{
		render_->Draw(renderTextureID);
	}

	ImGui::Begin("HaveItem");
	ImGui::DragFloat3("Item Position", &itemTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("Item Rotation", &itemTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Item Scale", &itemTransform_.scale.x, 0.1f);
	ImGui::Separator();
	ImGui::DragFloat3("pivot Position", &pivotTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("pivot Rotation", &pivotTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("pivot Scale", &pivotTransform_.scale.x, 0.1f);

	ImGui::End();
}

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

		const float scaleX = std::sqrt(worldMatrix.m[0][0] * worldMatrix.m[0][0] + worldMatrix.m[0][1] * worldMatrix.m[0][1] + worldMatrix.m[0][2] * worldMatrix.m[0][2]);
		const float scaleY = std::sqrt(worldMatrix.m[1][0] * worldMatrix.m[1][0] + worldMatrix.m[1][1] * worldMatrix.m[1][1] + worldMatrix.m[1][2] * worldMatrix.m[1][2]);
		const float scaleZ = std::sqrt(worldMatrix.m[2][0] * worldMatrix.m[2][0] + worldMatrix.m[2][1] * worldMatrix.m[2][1] + worldMatrix.m[2][2] * worldMatrix.m[2][2]);
		const float uniformScale = std::max({ scaleX, scaleY, scaleZ });

		for (const auto& localSphere : localSpheres)
		{
			Vector3 worldCenter = Transform(localSphere.center, worldMatrix);
			float worldRadius = localSphere.radius * uniformScale;
			result.push_back({ worldCenter, worldRadius });
		}
		return result;
	}
}


HaveItem::HaveItem()
{
	render_ = std::make_unique<RenderObject>();
	render_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModelNonIASet.VS.hlsl";
	render_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	render_->SetupFromShaders();

	pivotTransform_.translate = { 0.0f, 0.0f, 0.0f };
	pivotTransform_.rotate = { 0.0f, 0.0f, 0.0f };
	pivotTransform_.scale = { 1.0f, 1.0f, 1.0f };
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
//		render_->SetBRegisterData(0, ShaderType::VertexShader, &wvp);
//		render_->SetBRegisterData(1, ShaderType::VertexShader, &itemWorld);
//		render_->SetBRegisterData(0, ShaderType::PixelShader, &color);
//		render_->SetBRegisterData(1, ShaderType::PixelShader, &textureID);
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

void HaveItem::SetItem(ItemID itemID)
{
	if (itemID == currentItemID_)
	{
		return;
	}

	currentItemID_ = itemID;
	itemInfo_ = App::Data::Item::Get(currentItemID_);
	if (itemInfo_)
	{
		render_->modelID_ = itemInfo_->modelID;
		t_haveItem_ = itemInfo_->textureID;
		modelData_ = Game::Asset::Model::GetData(itemInfo_->modelID);
		a_haveItem_ = Game::Asset::Animation::Load("assets/application/Minecraft/Item/tool/hammer/hammer.gltf", "Animation");
	}
	else
	{
		render_->modelID_ = -1;
		worldCollider_.spheres.clear();
		worldCollider_.aabbs.clear();
	}
}

void HaveItem::Update(int32_t cameraID)
{
	//if (Game::IO::Mouse::IsHeld(0))
	//{
	//	if (itemInfo_)
	//	{
	//		animationTime_ += Game::Time::GetScaledDeltaTimeMs() * 0.001f;
	//		Matrix4x4 itemAnimWorld = Game::Asset::Animation::SampleNodeHierarchy(a_haveItem_, "Untitled", animationTime_);
	//
	//		Vector3 cameraDir = Game::Camera::Getter::GetCameraDirection(cameraID);
	//		pivotTransform_.rotate = Game::Math::YawPitchFromDirection(cameraDir);
	//		Matrix4x4 pivotWorld = Matrix4x4::MakeAffineMatrix(pivotTransform_.scale, pivotTransform_.rotate, pivotTransform_.translate);
	//		worldMatrix_ = itemAnimWorld * pivotWorld * parentWorldMatrix_;
	//		wvpMatrix_ = worldMatrix_ * Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
	//
	//		worldCollider_.spheres = CreateSphere(modelData_->colliderShape.spheres, worldMatrix_);
	//		worldCollider_.aabbs = CreateAABB(modelData_->colliderShape.aabbs, worldMatrix_);
	//	}
	//}
	//else
	//{
	//	animationTime_ = 0.0f;
	//}
	if (itemInfo_)
	{
		if (Game::IO::Mouse::IsHeld(0))
		{
			animationTime_ += Game::Time::GetScaledDeltaTimeMs() * 0.001f * animationSpeed_;
		}
		else
		{
			animationTime_ = 0.0f;
		}

		Matrix4x4 itemAnimWorld = Game::Asset::Animation::SampleNodeHierarchy(a_haveItem_, "Untitled", animationTime_);

		Vector3 cameraDir = Game::Camera::Getter::GetCameraDirection(cameraID);
		pivotTransform_.rotate = Game::Math::YawPitchFromDirection(cameraDir);
		Matrix4x4 pivotWorld = Matrix4x4::MakeAffineMatrix(pivotTransform_.scale, pivotTransform_.rotate, pivotTransform_.translate);
		worldMatrix_ = itemAnimWorld * pivotWorld * parentWorldMatrix_;
		wvpMatrix_ = worldMatrix_ * Game::Camera::Getter::GetViewProjectionMatrix(cameraID);

		worldCollider_.spheres = CreateSphere(modelData_->colliderShape.spheres, worldMatrix_);
		worldCollider_.aabbs = CreateAABB(modelData_->colliderShape.aabbs, worldMatrix_);
	}
}

void HaveItem::Draw(int32_t renderTextureID)
{
	if (render_->modelID_ >= 0)
	{
		render_->SetTRegisterData(0, ShaderType::VertexShader, modelData_->vertexHeapSlot);
		render_->SetBRegisterData(0, ShaderType::VertexShader, &wvpMatrix_);
		render_->SetBRegisterData(1, ShaderType::VertexShader, &worldMatrix_);
		render_->SetBRegisterData(0, ShaderType::PixelShader, &color);
		render_->SetBRegisterData(1, ShaderType::PixelShader, &t_haveItem_);
		render_->Draw(renderTextureID);
	}
}


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

	modelTransform_.translate = { 0.0f, 0.0f, 0.0f };
	modelTransform_.rotate = { 1.570f, 0.0f, -0.785f };
	modelTransform_.scale = { 1.0f, 1.0f, 1.0f };

	itemTransform_.translate = { 0.0f, 0.0f, -1.5f };
	itemTransform_.rotate = { 0.0f, 0.0f, 0.0f };
	itemTransform_.scale = { 1.0f, 1.0f, 1.0f };
	//itemTransform_.translate = { 0.65f, -0.620f, 0.0f };
	//itemTransform_.rotate = { -2.57f, 0.0f, -2.35619f };

	pivotTransform_.translate = { 0.0f, 0.0f, 0.0f };
	pivotTransform_.rotate = { 0.0f, 0.0f, 0.0f };
	pivotTransform_.scale = { 1.0f, 1.0f, 1.0f };

	preTransform_ = itemTransform_;
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
	if (stage_ == 0 && Game::IO::Mouse::IsHeld(0) && !Game::IO::Key::IsHeld(VK_LSHIFT))
	{
		startTime_ = 0.0f;
		stage_ = 1;


		//itemTransform_.rotate.x = 0.75f;
		//itemTransform_.rotate.y = 1.5f;
		//itemTransform_.rotate.z = 0.0f;
		//itemTransform_.translate.x = 2.0f;
		//itemTransform_.translate.y = 2.0f;
		//itemTransform_.translate.z = 0.0f;
	}

	if (stage_ == 1)
	{
		startTime_ += Game::Time::GetDeltaTimeMs();
		float t = startTime_ / 100.0f;
		if (t > 1.0f)
		{
			t = 1.0f;
			startTime_ = 0.0f;
			stage_ = 2;
		}
		itemTransform_.translate = Game::Math::Ease::Easing(preTransform_.translate, Vector3{ 2.0f, 2.0f, 0.0f }, EaseType::LINEAR, t);
		itemTransform_.rotate = Game::Math::Ease::Easing(preTransform_.rotate, Vector3{ 0.75f, 1.5f, 0.0f }, EaseType::LINEAR, t);
	}
	else if (stage_ == 2)
	{
		startTime_ += Game::Time::GetDeltaTimeMs();
		float t = startTime_ / 1000.0f;
		if (t > 1.0f)
		{
			t = 1.0f;
			startTime_ = 0.0f;
			stage_ = 3;
		}
		rotate = Game::Math::Ease::Easing(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ -3.0f,-3.0f,0.0f }, EaseType::LINEAR, t);
		//itemTransform_.translate = Game::Math::Ease::Easing(Vector3{ 2.0f,2.0f,0.0f }, Vector3{ -2.0f,-2.0f,0.0f }, EaseType::LINEAR, t);
	}
	else if (stage_ == 3)
	{
		startTime_ += Game::Time::GetDeltaTimeMs();
		float t = startTime_ / 500.0f;
		if (t > 1.0f)
		{
			t = 1.0f;
			startTime_ = 0.0f;
			stage_ = 0;
		}
		itemTransform_.translate = Game::Math::Ease::Easing(Vector3{ 2.0f,2.0f,0.0f }, preTransform_.translate, EaseType::LINEAR, t);
		itemTransform_.rotate = Game::Math::Ease::Easing(Vector3{ 0.75f,1.5f,0.0f }, preTransform_.rotate, EaseType::LINEAR, t);
		rotate = Game::Math::Ease::Easing(Vector3{ -3.0f,-3.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, EaseType::LINEAR, t);
	}

		
	if (currentItemID_ != ItemID::MAX)
	{
		const ItemInfo* itemInfo = App::Data::Item::Get(currentItemID_);
		if (!itemInfo)
		{
			__debugbreak();
			return;
		}

		Vector3 cameraDir = Game::Camera::Getter::GetCameraDirection(cameraID);
		pivotTransform_.rotate = Game::Math::YawPitchFromDirection(cameraDir);


		pivotTransform_.rotate += rotate;



		Matrix4x4 modelWorld = Matrix4x4::MakeAffineMatrix(modelTransform_.scale, modelTransform_.rotate, modelTransform_.translate);
		Matrix4x4 itemWorld = Matrix4x4::MakeAffineMatrix(itemTransform_.scale, itemTransform_.rotate, itemTransform_.translate);
		Matrix4x4 pivotWorld = Matrix4x4::MakeAffineMatrix(pivotTransform_.scale, pivotTransform_.rotate, pivotTransform_.translate);
		Matrix4x4 world = modelWorld * itemWorld * pivotWorld * parentWorldMatrix_;
		Matrix4x4 wvp = world * Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		int32_t textureID = itemInfo->textureID;
		render_->modelID_ = itemInfo->modelID;

		render_->SetCBufferData(0, ShaderType::VertexShader, &wvp);
		render_->SetCBufferData(1, ShaderType::VertexShader, &world);
		render_->SetCBufferData(0, ShaderType::PixelShader, &color);
		render_->SetCBufferData(1, ShaderType::PixelShader, &textureID);

		const ModelData* modelData = Game::Asset::Model::GetData(itemInfo->modelID);
		worldCollider_.spheres = CreateSphere(modelData->colliderShape.spheres, world);
		worldCollider_.aabbs = CreateAABB(modelData->colliderShape.aabbs, world);
	}
	else
	{
		render_->modelID_ = -1;
		worldCollider_.spheres.clear();
		worldCollider_.aabbs.clear();
	}
}

void HaveItem::Draw(int32_t renderTextureID)
{
	if (render_->modelID_ >= 0)
	{
		render_->Draw(renderTextureID);
	}

		ImGui::Begin("HaveIte");
		ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
		ImGui::End();

	ImGui::Begin("HaveItem");
	ImGui::DragFloat3("Item Position", &itemTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("Item Rotation", &itemTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Item Scale", &itemTransform_.scale.x, 0.1f);
	//ImGui::Separator();
	//ImGui::DragFloat3("model Position", &modelTransform_.translate.x, 0.1f);
	//ImGui::DragFloat3("model Rotation", &modelTransform_.rotate.x, 0.01f);
	//ImGui::DragFloat3("model Scale", &modelTransform_.scale.x, 0.1f);
	ImGui::Separator();
	ImGui::DragFloat3("pivot Position", &pivotTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("pivot Rotation", &pivotTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("pivot Scale", &pivotTransform_.scale.x, 0.1f);

	ImGui::End();
}

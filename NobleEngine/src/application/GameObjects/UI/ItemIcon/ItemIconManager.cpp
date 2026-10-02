#include "ItemIconManager.h"
#include <App.h>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <numbers>
#include <string>

namespace
{
	// 視野角・クリップ(エンジンのCameraの初期値と同じ)
	constexpr float kIconFovY = 0.65f;
	constexpr float kIconNearZ = 0.01f;
	constexpr float kIconFarZ = 500.0f;
	// デフォルトモデル
	const std::string kDefaultModelPath = "assets/engine/model/cube/cube.obj";
	// デフォルトテクスチャ
	const std::string kWhiteTexturePath = "assets/engine/texture/white1x1.png";
}

ItemIconManager::ItemIconManager()
{}

ItemIconManager::~ItemIconManager()
{}

int32_t ItemIconManager::GetIcon(ItemID id)
{
	if (id == ItemID::MAX) return -1;

	Icon& icon = icons_[static_cast<size_t>(id)];

	// 初めて頼まれたときにレンダーテクスチャを作る
	if (icon.renderTextureID < 0)
	{
		const std::string label = "ItemIcon_" + std::to_string(static_cast<int32_t>(id));
		icon.renderTextureID = Game::Asset::RenderTexture::CreateRenderTexture(kIconSize, kIconSize, label);
		icon.render = CreateRenderObject();
		needsDrawIcons_[id] = &icon;
	}

	return icon.renderTextureID;
}

void ItemIconManager::Draw()
{
	for (const auto& [itemID, icon] : needsDrawIcons_)
	{
		if (!icon) continue;
		const ItemInfo* info = App::Data::Item::Get(itemID);
		if (!info) continue;

		DrawItem(*icon->render, *info, icon->renderTextureID);
	}

	needsDrawIcons_.clear();
}

std::unique_ptr<RenderObject> ItemIconManager::CreateRenderObject()
{
	auto render = std::make_unique<RenderObject>();
	render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	render->psoConfig_.ps = "assets/shaders/UI/ItemIcon.PS.hlsl";
	render->SetupFromShaders();
	return render;
}

void ItemIconManager::DrawItem(RenderObject& render, const ItemInfo& info, int32_t renderTextureID)
{
	// モデル。設定されていなければCube
	int32_t modelID = info.modelID;
	if (modelID < 0)
	{
		modelID = Game::Asset::Model::Load(kDefaultModelPath);
	}
	const ModelData* model = Game::Asset::Model::GetData(modelID);
	if (!model) return;
	render.modelID_ = modelID;

	// 色とテクスチャ。ブロックは色のみ
	Vector4 color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	int32_t textureID = info.textureID;
	if (info.genre == ItemGenre::Block)
	{
		if (const BlockInfo* block = App::Data::Item::Get(info.blockID))
		{
			color = Game::Math::Converter::UintToVector4(block->color);
		}
		textureID = Game::Asset::Texture::Load(kWhiteTexturePath);
	}
	if (textureID < 0) return;

	// モデルは原点に置いたまま、カメラの方を動かす
	Matrix4x4 world = Matrix4x4::MakeIdentity4x4();
	Matrix4x4 wvp = world * MakeViewProjection(info, *model);

	render.SetBRegisterData(0, ShaderType::VertexShader, &wvp);
	render.SetBRegisterData(1, ShaderType::VertexShader, &world);
	render.SetBRegisterData(0, ShaderType::PixelShader, &color);
	render.SetBRegisterData(1, ShaderType::PixelShader, &textureID);
	render.Draw(renderTextureID);
}

Matrix4x4 ItemIconManager::MakeViewProjection(const ItemInfo& info, const ModelData& model)
{
	Vector3 center = info.cameraPos;

	// 距離が0以下なら、モデル全体がちょうど画面に収まる距離にする
	float distance = info.iconCamera.radius;
	if (distance <= 0.0f)
	{
		float radius = 1.0f;
		if (!model.vertices.empty())
		{
			Vector3 minPos = Vector3{ FLT_MAX, FLT_MAX, FLT_MAX };
			Vector3 maxPos = Vector3{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
			for (const VertexData& vertex : model.vertices)
			{
				minPos.x = std::min(minPos.x, vertex.position.x);
				minPos.y = std::min(minPos.y, vertex.position.y);
				minPos.z = std::min(minPos.z, vertex.position.z);
				maxPos.x = std::max(maxPos.x, vertex.position.x);
				maxPos.y = std::max(maxPos.y, vertex.position.y);
				maxPos.z = std::max(maxPos.z, vertex.position.z);
			}
			center = (minPos + maxPos) * 0.5f;
			radius = std::max((maxPos - minPos).Length() * 0.5f, 0.001f);
		}
		distance = radius / std::sin(kIconFovY * 0.5f);
	}

	// カメラを生成
	const float phiLimit = std::numbers::pi_v<float> *0.5f - 0.001f;
	const float phi = std::clamp(info.iconCamera.phi, -phiLimit, phiLimit);
	const float cosPhi = std::cos(phi);
	const Vector3 eye = center + Vector3{ distance * cosPhi * std::cos(info.iconCamera.theta), distance * std::sin(phi), distance * cosPhi * std::sin(info.iconCamera.theta) };

	const Matrix4x4 view = Matrix4x4::LookAtMatrix(eye, center, Vector3{ 0.0f, 1.0f, 0.0f });
	const Matrix4x4 projection = Matrix4x4::MakePerspectiveFovMatrix(kIconFovY, 1.0f, kIconNearZ, kIconFarZ);
	return view * projection;
}
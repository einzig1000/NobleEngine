#include "HaveItem.h"
#include <definition/ItemConfig/ItemConfig.h>
#include <Utilities/functions.h>

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
			for (int i = 1; i < 8; ++i)
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
}


HaveItem::HaveItem()
{
	rightHandItemRenderObject_ = std::make_unique<RenderObject>();
	rightHandItemRenderObject_->psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModel.VS.hlsl";
	rightHandItemRenderObject_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	rightHandItemRenderObject_->SetupFromShaders();
}

HaveItem::~HaveItem()
{}

void HaveItem::Update(int32_t cameraID)
{
    if (currentItemID != ItemID::None)
    {
        const ItemInfo& itemConfig = ItemConfig::Instance().GetItemInfo(currentItemID);

        rightHandItemRenderObject_->modelID_ = itemConfig.modelID;
		Matrix4x4 world = Matrix4x4::MakeTranslateMatrix(*charactorPosition_);
        Matrix4x4 wvp = world * Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
        Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        int32_t textureID = itemConfig.textureID;

        rightHandItemRenderObject_->SetCBufferData(0, ShaderType::VertexShader, &wvp);
        rightHandItemRenderObject_->SetCBufferData(1, ShaderType::VertexShader, &world);
        rightHandItemRenderObject_->SetCBufferData(0, ShaderType::PixelShader, &color);
        rightHandItemRenderObject_->SetCBufferData(1, ShaderType::PixelShader, &textureID);

		itemAABB_ = CreateAABB(itemAABB_, world);
    }
    else
    {
        rightHandItemRenderObject_->modelID_ = -1;
    }
}

void HaveItem::Draw(int32_t renderTextureID)
{
	if (rightHandItemRenderObject_->modelID_ >= 0)
	{
		rightHandItemRenderObject_->Draw(renderTextureID);
	}
}

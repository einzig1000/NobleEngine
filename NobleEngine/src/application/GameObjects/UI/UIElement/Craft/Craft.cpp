#include "Craft.h"
#include <Game.h>

Craft::Craft()
{
	// sprites_[0] : アイテムインベントリ
	sprites_.emplace_back(ElementData{});
	sprites_[0].render = std::make_unique<RenderObject>();
	sprites_[0].render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	sprites_[0].render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprites_[0].render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	sprites_[0].render->SetupFromShaders();
	sprites_[0].textureID = 0;//ResourceID::GetUITextureID(UITextureID::Inventory3x3);
	sprites_[0].transforms.scale = Vector3(1.0f, 1.0f, 1.0f);
	sprites_[0].transforms.translate = Vector3(640.0f, 325.0f, 0.0f);
}

Craft::~Craft()
{}

void Craft::Initialize()
{}

void Craft::Update(int32_t cameraID)
{
	orthographic_ = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);
}

void Craft::Draw(int32_t rt_ID)
{
	for (const auto& sprite : sprites_)
	{
		for (const auto& sprite : sprites_)
		{
			Matrix4x4 worldMatrix_ = Matrix4x4::MakeAffineMatrix(sprite.transforms.scale, sprite.transforms.rotate, sprite.transforms.translate);
			Matrix4x4 wvpMatrix_ = worldMatrix_ * orthographic_;
			Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

			sprite.render->SetBRegisterData(0, ShaderType::VertexShader, &wvpMatrix_);
			sprite.render->SetBRegisterData(1, ShaderType::VertexShader, &worldMatrix_);
			sprite.render->SetBRegisterData(0, ShaderType::PixelShader, &color);
			sprite.render->SetBRegisterData(1, ShaderType::PixelShader, &sprite.textureID);
			sprite.render->Draw(rt_ID);
		}
	}
}
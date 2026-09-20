#include "Inventory.h"

Inventory::Inventory()
{
	// sprites_[0] : アイテムインベントリ
	sprites_.emplace_back(ElementData{});
	sprites_[0].render = std::make_unique<RenderObject>();
	sprites_[0].render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	sprites_[0].render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprites_[0].render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	sprites_[0].render->SetupFromShaders();
	sprites_[0].textureID = Game::Asset::Texture::Load("assets/application/Minecraft/UI/Inventory/Inventory2x2.png");
	const TextureData* textureData = Game::Asset::Texture::GetData(sprites_[0].textureID);
	sprites_[0].transforms.scale = Vector3(float(textureData->metadata.width) / 2.0f, float(textureData->metadata.height) / 2.0f, 1.0f);
	sprites_[0].transforms.translate = Vector3(640.0f, 360.0f, 1.0f);
	sprites_[0].transforms.rotate = Vector3(0.0f, 0.0f, 0.0f);
}

Inventory::~Inventory()
{}

void Inventory::Initialize()
{}

void Inventory::Update(int32_t cameraID)
{
	Matrix4x4 orthographic = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);
	for (const auto& sprite : sprites_)
	{
		worldMatrix_ = Matrix4x4::MakeAffineMatrix(sprite.transforms.scale, sprite.transforms.rotate, sprite.transforms.translate);
		wvpMatrix_ = worldMatrix_ * orthographic;
	}
}

void Inventory::Draw(int32_t rt_ID)
{
	for (const auto& sprite : sprites_)
	{
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		sprite.render->SetBRegisterData(0, ShaderType::VertexShader, &wvpMatrix_);
		sprite.render->SetBRegisterData(1, ShaderType::VertexShader, &worldMatrix_);
		sprite.render->SetBRegisterData(0, ShaderType::PixelShader, &color);
		sprite.render->SetBRegisterData(1, ShaderType::PixelShader, &sprite.textureID);
		sprite.render->Draw(rt_ID);
	}
}

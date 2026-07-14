#include "Hotbar.h"

Hotbar::Hotbar()
{
	// sprites_[0] : ホットバー
	sprites_.emplace_back(ElementData{});
	sprites_[0].render = std::make_unique<RenderObject>();
	sprites_[0].render->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	sprites_[0].render->psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprites_[0].render->modelID_ = Game::Asset::Model::Load("resources/prototypes/model/plane/plane.obj");
	sprites_[0].render->SetupFromShaders();
	sprites_[0].textureID = Game::Asset::Texture::Load("resources/Minecraft/UI/Inventory/Hotbar.png");
	TextureData* textureData = Game::Asset::Texture::GetData(sprites_[0].textureID);
	sprites_[0].transforms.scale = Vector3(textureData->metadata.width, textureData->metadata.height, 1.0f);
	sprites_[0].transforms.translate = Vector3(640.0f, 670.0f, 1.0f);
}

Hotbar::~Hotbar()
{}

void Hotbar::Initialize()
{}

void Hotbar::Update(int32_t cameraID)
{
	Matrix4x4 orthographic = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);
	for (const auto& sprite : sprites_)
	{
		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(sprite.transforms.scale, sprite.transforms.rotate, sprite.transforms.translate);
		Matrix4x4 wvp = world * orthographic;
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

		sprite.render->SetCBufferData(0, ShaderType::VertexShader, &wvp);
		sprite.render->SetCBufferData(1, ShaderType::VertexShader, &world);
		sprite.render->SetCBufferData(0, ShaderType::PixelShader, &color);
		sprite.render->SetCBufferData(1, ShaderType::PixelShader, &sprite.textureID);
	}
}

void Hotbar::Draw(int32_t rt_ID)
{
	for (const auto& sprite : sprites_)
	{
		sprite.render->Draw(rt_ID);
	}

	ImGui::Begin("Hotbar");
	ImGui::DragFloat3("Position", &sprites_[0].transforms.translate.x);
	ImGui::DragFloat3("Scale", &sprites_[0].transforms.scale.x);
	ImGui::End();
}

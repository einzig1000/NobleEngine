#include "Pause.h"

Pause::Pause()
{
	// sprites_[0] : ポーズメニュー
	sprites_.emplace_back(ElementData{});
	sprites_[0].render = std::make_unique<RenderObject>();
	sprites_[0].render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	sprites_[0].render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprites_[0].render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	sprites_[0].render->SetupFromShaders();
	sprites_[0].textureID = 0;
	sprites_[0].transforms.scale = Vector3(1.0f, 1.0f, 1.0f);
	sprites_[0].transforms.translate = Vector3(640.0f, 325.0f, 0.0f);

	// sprites_[1] : 保存ボタン
	sprites_.emplace_back(ElementData{});
	sprites_[1].render = std::make_unique<RenderObject>();
	sprites_[1].render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	sprites_[1].render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprites_[1].render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	sprites_[1].render->SetupFromShaders();
	sprites_[1].textureID = 0;
	sprites_[1].transforms.scale = Vector3(1.0f, 1.0f, 1.0f);
	sprites_[1].transforms.translate = Vector3(640.0f, 325.0f, 0.0f);

	const TextureData* textureData = Game::Asset::Texture::GetData(sprites_[1].textureID);
	//buttonSize_ = Vector2(textureData->metadata.width, textureData->metadata.height);
}

Pause::~Pause()
{}

void Pause::Initialize()
{}

void Pause::Update(int32_t cameraID)
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

	Vector2 mousePos = Game::IO::Mouse::Get2DPosition();

	//if (sprites_[1]->isCollisionMouseRay)
	//{
	//	sprites_[1]->color = 0xFFFFFFFF;
	//	if (Game::IO::Mouse::IsJustPressed(0))
	//	{
	//		saveRequested_ = true;	
	//	}
	//}
	//else
	//{
	//	sprites_[2]->color = 0x777777FF;
	//}
}

void Pause::Draw(int32_t rt_ID)
{
	for (const auto& sprite : sprites_)
	{
		sprite.render->Draw(rt_ID);
	}
}

bool Pause::ConsumeSaveRequested()
{
	if (!saveRequested_) return false;
	saveRequested_ = false;
	return true;
}

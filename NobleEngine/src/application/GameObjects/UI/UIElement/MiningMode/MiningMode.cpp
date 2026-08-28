#include "MiningMode.h"
#include <GameObjects/EventBus/EventBus.h>

MiningMode::MiningMode()
{
	// sprites_[0] : 
	sprites_.emplace_back(ElementData{});
	sprites_[0].render = std::make_unique<RenderObject>();
	sprites_[0].render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	sprites_[0].render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprites_[0].render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	sprites_[0].render->SetupFromShaders();
	sprites_[0].textureID = Game::Asset::Texture::Load("assets/application/Minecraft/UI/MiningMode/mode1.png");
	const TextureData* textureData = Game::Asset::Texture::GetData(sprites_[0].textureID);
	sprites_[0].transforms.scale = Vector3(float(textureData->metadata.width) / 2.0f, float(textureData->metadata.height) / -2.0f, 1.0f);
	sprites_[0].transforms.translate = Vector3(280.0f, 360.0f, 1.0f);

	// sprites_[1] : 
	sprites_.emplace_back(ElementData{});
	sprites_[1].render = std::make_unique<RenderObject>();
	sprites_[1].render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	sprites_[1].render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprites_[1].render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	sprites_[1].render->SetupFromShaders();
	sprites_[1].textureID = Game::Asset::Texture::Load("assets/application/Minecraft/UI/MiningMode/mode2.png");
	const TextureData* textureData2 = Game::Asset::Texture::GetData(sprites_[1].textureID);
	sprites_[1].transforms.scale = Vector3(float(textureData2->metadata.width) / 2.0f, float(textureData2->metadata.height) / -2.0f, 1.0f);
	sprites_[1].transforms.translate = Vector3(static_cast<float>(Game::Window::GetWidth()) - 280.0f, 360.0f, 1.0f);
}

MiningMode::~MiningMode()
{}

void MiningMode::Initialize()
{}

void MiningMode::Update(int32_t cameraID)
{
	float rotateZ = 0.0f;
	uint32_t time = Game::Time::GetElapsedTime();
	rotateZ = std::sinf(static_cast<float>(time) / 10.0f) * 0.2f;

	Vector2 mousePos = Game::IO::Mouse::Get2DPosition();
	bool leftKey = mousePos.x < static_cast<float>(Game::Window::GetWidth()) / 2.0f;

	if (leftKey)
	{
		sprites_[0].transforms.rotate.z = rotateZ;
		sprites_[1].transforms.rotate.z = 0.0f;
	}
	else
	{
		sprites_[0].transforms.rotate.z = 0.0f;
		sprites_[1].transforms.rotate.z = rotateZ;
	}

	if (Game::IO::Mouse::IsJustPressed(0))
	{
		Event event;
		event.type = EventType::MiningModeChanged;
		event.value.push_back(leftKey ? 0 : 1);

		if (leftKey)
		{
			eventBus_->Notify(event);
		}
		else
		{
			eventBus_->Notify(event);
		}

		*nextUIMode_ = UIMode::Playing;
	}

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

void MiningMode::Draw(int32_t rt_ID)
{
	for (const auto& sprite : sprites_)
	{
		sprite.render->Draw(rt_ID);
	}

	ImGui::Begin("MiningMode");
	ImGui::DragFloat3("Position1", &sprites_[0].transforms.translate.x);
	ImGui::DragFloat3("Scale1", &sprites_[0].transforms.scale.x);
	ImGui::DragFloat3("rotate1", &sprites_[0].transforms.rotate.x);
	ImGui::DragFloat3("Position2", &sprites_[1].transforms.translate.x);
	ImGui::DragFloat3("Scale2", &sprites_[1].transforms.scale.x);
	ImGui::End();
}

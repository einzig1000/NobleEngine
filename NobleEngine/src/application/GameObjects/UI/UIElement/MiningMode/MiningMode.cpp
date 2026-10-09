#include "MiningMode.h"
#include <System/EventBus/EventBus.h>

MiningMode::MiningMode()
{
	Vector2 windowSize = Vector2(float(Game::Window::GetWidth()), float(Game::Window::GetHeight()));

	// sprites_[0] : 採掘モードアイコン１
	sprites_.emplace_back(ElementData{});
	sprites_[0].render = std::make_unique<RenderObject>();
	sprites_[0].render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	sprites_[0].render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprites_[0].render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	sprites_[0].render->SetupFromShaders();
	sprites_[0].textureID = Game::Asset::Texture::Load("assets/application/texture/UI/MiningMode/mode1.png");
	const TextureData* textureData = Game::Asset::Texture::GetData(sprites_[0].textureID);
	sprites_[0].transforms.scale = Vector3(float(textureData->metadata.width) / 2.0f, float(textureData->metadata.height) / -2.0f, 1.0f);
	sprites_[0].transforms.translate = Vector3(windowSize.x * 0.3f, windowSize.y * 0.5f, 1.0f);

	// sprites_[1] : 採掘モードアイコン２
	sprites_.emplace_back(ElementData{});
	sprites_[1].render = std::make_unique<RenderObject>();
	sprites_[1].render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	sprites_[1].render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprites_[1].render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	sprites_[1].render->SetupFromShaders();
	sprites_[1].textureID = Game::Asset::Texture::Load("assets/application/texture/UI/MiningMode/mode2.png");
	const TextureData* textureData2 = Game::Asset::Texture::GetData(sprites_[1].textureID);
	sprites_[1].transforms.scale = Vector3(float(textureData2->metadata.width) / 2.0f, float(textureData2->metadata.height) / -2.0f, 1.0f);
	sprites_[1].transforms.translate = Vector3(windowSize.x * 0.7f, windowSize.y * 0.5f, 1.0f);
}

MiningMode::~MiningMode()
{}

void MiningMode::Initialize()
{}

void MiningMode::Update(int32_t cameraID)
{
	orthographic_ = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);


	float rotateZ = 0.0f;
	float elapsedTime = Game::Time::GetElapsedSecTime();
	rotateZ = std::sinf(elapsedTime) * 0.2f;

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
		MiningPattern pattern = leftKey ? MiningPattern::Swing : MiningPattern::Range;
		event.data = pattern;

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
}

void MiningMode::Draw(int32_t rt_ID)
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

void MiningMode::DrawImGui()
{
	ImGui::Begin("MiningMode");
	ImGui::DragFloat3("Position1", &sprites_[0].transforms.translate.x);
	ImGui::DragFloat3("Scale1", &sprites_[0].transforms.scale.x);
	ImGui::DragFloat3("rotate1", &sprites_[0].transforms.rotate.x);
	ImGui::DragFloat3("Position2", &sprites_[1].transforms.translate.x);
	ImGui::DragFloat3("Scale2", &sprites_[1].transforms.scale.x);
	ImGui::DragFloat3("rotate2", &sprites_[1].transforms.rotate.x);
	ImGui::End();
}

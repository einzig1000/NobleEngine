#include "Test2Phase.h"
#include <Game.h>

Test2Phase::Test2Phase()
{
	c_main_ = Game::Camera::AddCamera();
	Game::Camera::Setter::SetDistance(5.0f, 0, EaseType::IN_BACK, c_main_);
	Game::Camera::Setter::SetEnableControl(true, c_main_);

	t_dissolveMaskTexture_ = Game::Asset::Texture::Load("resources/prototypes/texture/noise0.png");

	rt_main_ = Game::Asset::RenderTexture::CreateRenderTexture(1280, 720, "main");
	rt_noise_ = Game::Asset::RenderTexture::CreateRenderTexture(1280, 720, "noise");

	render_ = std::make_unique<RenderObject>();
	//render_->psoConfig_.ps = "resources/shaders/FullScreen/LuminanceBasedOutline.PS.hlsl";
	render_->psoConfig_.ps = "resources/shaders/FullScreen/Dissolve.PS.hlsl";
	//render_->psoConfig_.ps = "resources/shaders/FullScreen/RandomNoise.PS.hlsl";
	render_->psoConfig_.vs = "resources/shaders/FullScreen/FullScreen.VS.hlsl";
	render_->modelID_ = Game::Asset::Model::Load("resources/prototypes/model/plane/plane.obj");
	render_->SetupFromShaders();
}

Test2Phase::~Test2Phase()
{}

void Test2Phase::Initialize()
{
	testParticle.Initialize();
	testAnimation.Initialize();
}

void Test2Phase::Update()
{
	Game::Camera::Update(c_main_);

	testParticle.Update(c_main_);
	//testAnimation.Update(c_main_);

	if (Game::IO::Key::IsJustPressed(VK_F11))
	{
		Game::Asset::RenderTexture::SaveAllRenderTextureToFile("screenshots");
	}

	frame_ += 1.0f;

	// Dissolveの時
	render_->SetCBufferData(0, ShaderType::PixelShader, &rt_main_);
	render_->SetCBufferData(1, ShaderType::PixelShader, &t_dissolveMaskTexture_);
	render_->SetCBufferData(2, ShaderType::PixelShader, &threshold);
	Vector3 edgeColor = Vector3(1.0f, 0.0f, 0.0f);
	render_->SetCBufferData(3, ShaderType::PixelShader, &edgeColor);

	// RandomNoiseの時
	//render_->SetCBufferData(0, ShaderType::PixelShader, &frame_);
}

void Test2Phase::Draw()
{
	testParticle.Draw(rt_main_);
	//testAnimation.Draw(rt_main_);

	render_->ScreenDraw();
}

void Test2Phase::DrawImGui()
{
	ImGui::Begin("Test2Phase"); 
	ImGui::DragFloat("Threshold", &threshold, 0.01f, 0.0f, 1.0f);
	ImGui::End();
}

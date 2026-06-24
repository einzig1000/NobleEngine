#include "Test2Phase.h"
#include <Game.h>

Test2Phase::Test2Phase()
{
	c_main_ = Game::Camera::AddCamera();
	Game::Camera::Setter::SetDistance(5.0f, 0, EaseType::IN_BACK, c_main_);
	Game::Camera::Setter::SetEnableControl(true, c_main_);

	t_dissolveMaskTexture_ = Game::Resource::Texture::Load("resources/prototypes/texture/noise0.png");

	rt_main_ = Game::Resource::CreateRenderTexture(1280, 720, "main");

	render_ = std::make_unique<RenderObject>();
	//render_->psoConfig_.ps = "resources/shaders/FullScreen/LuminanceBasedOutline.PS.hlsl";
	render_->psoConfig_.ps = "resources/shaders/FullScreen/Dissolve.PS.hlsl";
	render_->psoConfig_.vs = "resources/shaders/FullScreen/FullScreen.VS.hlsl";
	render_->modelID_ = Game::Resource::Model::Load("resources/prototypes/model/plane/plane.obj");
	render_->SetupFromShaders();
}

Test2Phase::~Test2Phase()
{}

void Test2Phase::Initialize()
{
	TestParticle.Initialize();
}

void Test2Phase::Update()
{
	Game::Camera::Update(c_main_);

	cameraRotate_.x = std::sinf(frame_ * 0.001f);
	cameraRotate_.y = std::cosf(frame_ * 0.001f) * 0.5f;
	Game::Camera::Setter::SetRotate(cameraRotate_, 0, EaseType::IN_BACK, c_main_);

	TestParticle.Update(c_main_);

	if (Game::IO::Key::IsJustPressed(DIK_F11))
	{
		Game::Resource::SaveAllRenderTextureToFile("screenshots");
	}

	frame_ += 1.0f;

	render_->SetCBufferData(0, ShaderType::PixelShader, &rt_main_);
	render_->SetCBufferData(1, ShaderType::PixelShader, &t_dissolveMaskTexture_);
	render_->SetCBufferData(2, ShaderType::PixelShader, &threshold);
	Vector3 edgeColor = Vector3(1.0f, 0.0f, 0.0f);
	render_->SetCBufferData(3, ShaderType::PixelShader, &edgeColor);
}

void Test2Phase::Draw()
{
	TestParticle.Draw(rt_main_);

	render_->ScreenDraw();
}

void Test2Phase::DrawImGui()
{

	ImGui::Begin("Test2Phase"); 
	ImGui::DragFloat("Threshold", &threshold, 0.01f, 0.0f, 1.0f);
	ImGui::End();
}

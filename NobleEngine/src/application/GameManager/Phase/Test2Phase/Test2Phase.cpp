#include "Test2Phase.h"
#include <Game.h>

Test2Phase::Test2Phase()
{
	c_main_ = Game::Camera::AddCamera();
	rt_main_ = Game::Resource::CreateRenderTexture(1280, 720, "NobleScreen");
	Game::Camera::Setter::SetDistance(5.0f, 0, EaseType::IN_BACK, c_main_);
	Game::Camera::Setter::SetEnableControl(true, c_main_);
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
}

void Test2Phase::Draw()
{
	TestParticle.Draw(rt_main_);
}

void Test2Phase::DrawImGui()
{}

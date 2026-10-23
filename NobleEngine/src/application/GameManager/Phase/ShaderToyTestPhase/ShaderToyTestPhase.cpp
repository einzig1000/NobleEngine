#include "ShaderToyTestPhase.h"
#include <Utilities/Json/JsonManager.h>

ShaderToyTestPhase::ShaderToyTestPhase()
{}

ShaderToyTestPhase::~ShaderToyTestPhase()
{}

void ShaderToyTestPhase::Initialize()
{
	draw_Seascape_ = std::make_unique<RenderObject>();
	draw_Seascape_->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	draw_Seascape_->psoConfig_.ps = "assets/shaders/FullScreen/ShaderToy/Seascape.PS.hlsl";
	draw_Seascape_->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	draw_Seascape_->SetupFromShaders();

	// 描画先(-1 = NobleScreen)は起動時のウィンドウサイズで作られている
	seascapeResolution_ = Vector2(static_cast<float>(Constexprs::kInitialWindowWidth), static_cast<float>(Constexprs::kInitialWindowHeight));
}

void ShaderToyTestPhase::Update()
{
	seascapeTime_ += Game::Time::GetScaledDeltaTimeMs() * 0.001f * seascapeSpeed_;
}

void ShaderToyTestPhase::Draw()
{
	draw_Seascape_->SetBRegisterData(0, ShaderType::PixelShader, &seascapeTime_);
	draw_Seascape_->SetBRegisterData(1, ShaderType::PixelShader, &seascapeResolution_);
	draw_Seascape_->Draw();
}

void ShaderToyTestPhase::DrawImGui()
{
	ImGui::Begin("Seascape");
	ImGui::DragFloat("Time", &seascapeTime_, 0.01f);
	ImGui::DragFloat("Speed", &seascapeSpeed_, 0.01f, 0.0f, 10.0f);
	ImGui::End();
}
#include "ShaderToyTestPhase.h"
#include <Utilities/Json/JsonManager.h>

ShaderToyTestPhase::ShaderToyTestPhase()
{}

ShaderToyTestPhase::~ShaderToyTestPhase()
{}

void ShaderToyTestPhase::Initialize()
{
	render_ = std::make_unique<RenderObject>();
	render_->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	render_->psoConfig_.ps = "assets/shaders/FullScreen/ShaderToy/Seascape.PS.hlsl";
	render_->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	render_->SetupFromShaders();

	seascapeResolution_ = Vector2(static_cast<float>(Constexprs::kInitialWindowWidth), static_cast<float>(Constexprs::kInitialWindowHeight));
}

void ShaderToyTestPhase::Update()
{
	seascapeTime_ += Game::Time::GetScaledDeltaTimeMs() * 0.001f * seascapeSpeed_;
}

void ShaderToyTestPhase::Draw()
{
	render_->SetBRegisterData(0, ShaderType::PixelShader, &seascapeTime_);
	render_->SetBRegisterData(1, ShaderType::PixelShader, &seascapeResolution_);
	render_->Draw();
}

void ShaderToyTestPhase::DrawImGui()
{
	ImGui::Begin("Seascape");
	ImGui::DragFloat("Time", &seascapeTime_, 0.01f);
	ImGui::DragFloat("Speed", &seascapeSpeed_, 0.01f, 0.0f, 10.0f);
	ImGui::End();
}
#include "TestMeshShader.h"
#include <Game.h>

TestMeshShader::TestMeshShader()
{
	render_ = std::make_unique<RenderObject>();
	render_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	render_->psoConfig_.ms = "resources/shaders/MSTest/MSTest.MS.hlsl";
	render_->SetupFromShaders();
}

TestMeshShader::~TestMeshShader()
{
	Vector4 color1 = { 1.0f, 0.0f, 0.0f, 1.0f };
	Vector4 t_uvChecker = { 1.0f, 1.0f, 1.0f, 1.0f };

	render_->SetCBufferData(0, ShaderType::PixelShader, &color1);
	render_->SetCBufferData(1, ShaderType::PixelShader, &t_uvChecker);
	//render_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
	//render_->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
}

void TestMeshShader::Initialize()
{}

void TestMeshShader::Update()
{}


void TestMeshShader::Draw(int32_t rtID)
{
	render_->Draw(rtID);
}
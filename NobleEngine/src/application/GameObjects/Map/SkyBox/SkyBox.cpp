#include "SkyBox.h"

SkyBox::SkyBox()
{
	render_ = std::make_unique<RenderObject>();
	render_->psoConfig_.vs = "assets/shaders/SkyBox/SkyBox.VS.hlsl";
	render_->psoConfig_.ps = "assets/shaders/SkyBox/SkyBox.PS.hlsl";
	render_->SetupFromShaders();
}

SkyBox::~SkyBox()
{}

void SkyBox::Initialize()
{
	render_->modelID_ = Game::Asset::Model::Load("assets/engine/model/cube/cube.obj");
	textureID_ = Game::Asset::Texture::Load("assets/application/texture/skybox.dds");
}

void SkyBox::Update(int32_t cameraID)
{
	Matrix4x4 viewMatrix = Game::Camera::Getter::GetViewMatrix(cameraID);
	Matrix4x4 projectionMatrix = Game::Camera::Getter::GetProjectionMatrix(cameraID);

	Matrix4x4 noTranslateViewMatrix = viewMatrix;
	noTranslateViewMatrix.m[3][0] = 0.0f;
	noTranslateViewMatrix.m[3][1] = 0.0f;
	noTranslateViewMatrix.m[3][2] = 0.0f;
	Matrix4x4 noTranslateViewProjection = noTranslateViewMatrix * projectionMatrix;

	render_->SetCBufferData(0, ShaderType::PixelShader, &textureID_);
	render_->SetCBufferData(0, ShaderType::VertexShader, &noTranslateViewProjection);
}

void SkyBox::Draw(int32_t renderTargetID)
{
	render_->Draw(renderTargetID);
}

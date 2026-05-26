#include "TestParticle.h"
#include <Game.h>

TestParticle::TestParticle()
{
	t_uvChecker = Game::Resource::LoadTexture("resources/Prototypes/texture/particle/circle2.png");
	//t_uvChecker = Game::Resource::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	renderObjects_.resize(10);
	color.resize(10);
	transforms.resize(10);
	for (int i = 0; i < 10; ++i)
	{
		renderObjects_[i] = std::make_unique<RenderObject>();
		renderObjects_[i]->modelID_ = Game::Resource::LoadModel("resources/Prototypes/model/plane.obj");
		renderObjects_[i]->psoConfig_.ps = "resources/Shaders/SimpleModel/SimpleModel.PS.hlsl";
		renderObjects_[i]->psoConfig_.vs = "resources/Shaders/SimpleModel/SimpleModel.VS.hlsl";
		renderObjects_[i]->psoConfig_.blendID = BlendStateID::Add;
		renderObjects_[i]->psoConfig_.depthStencilID = DepthStencilID::TestOnly;
		renderObjects_[i]->SetupFromShaders();
		color[i] = Vector4{ 1.0f,1.0f,1.0f,1.0f };
		transforms[i].scale = { 0.5f,6.0f,0.5f };
		transforms[i].translate = { 0.0f, 0.0f, 0.0f };
		transforms[i].rotate = { Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1), Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1), Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1) };
	}
}

TestParticle::~TestParticle()
{}

void TestParticle::Initialize()
{}

void TestParticle::Update()
{
	for (int i = 0; i < 10; ++i)
	{
		color[i].w -= 0.02f;
		if (Game::Math::Rand::RandInt(0, 50) == i)
		{
			color[i].w = 1.0f;
		}

		Matrix4x4 viewProjection = Game::Camera::Getter::GetCurrentViewProjectionMatrix();
		Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transforms[i].scale, transforms[i].rotate, transforms[i].translate);
		Matrix4x4 worldViewProjection = worldMatrix * viewProjection;
		renderObjects_[i]->SetCBufferData(0, ShaderType::PixelShader, &color[i]);
		renderObjects_[i]->SetCBufferData(1, ShaderType::PixelShader, &t_uvChecker);
		renderObjects_[i]->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
		renderObjects_[i]->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
	}
}

void TestParticle::Draw()
{
	uint32_t rtID = Game::Resource::GetRenderTextureID("Main");

	for (int i = 0; i < 10; ++i)
	{
		renderObjects_[i]->Draw(rtID);
		//renderObjects_[i]->ScreenDraw();
	}
}

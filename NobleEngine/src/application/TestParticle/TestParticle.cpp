#include "TestParticle.h"
#include <Game.h>

TestParticle::TestParticle()
{
	t_uvChecker = Game::Resource::LoadTexture("resources/Prototypes/texture/particle/circle2.png");
	t_gradationLine = Game::Resource::LoadTexture("resources/Prototypes/texture/particle/gradationLine.png");
	renderPlanes_.resize(10);
	planeColors.resize(10);
	planeTransforms.resize(10);
	for (int i = 0; i < 10; ++i)
	{
		renderPlanes_[i] = std::make_unique<RenderObject>();
		renderPlanes_[i]->modelID_ = Game::Resource::LoadModel("resources/Prototypes/model/plane.obj");
		renderPlanes_[i]->psoConfig_.ps = "resources/Shaders/SimpleModel/SimpleModel.PS.hlsl";
		renderPlanes_[i]->psoConfig_.vs = "resources/Shaders/SimpleModel/SimpleModel.VS.hlsl";
		renderPlanes_[i]->psoConfig_.blendID = BlendStateID::Add;
		renderPlanes_[i]->psoConfig_.depthStencilID = DepthStencilID::TestOnly;
		renderPlanes_[i]->SetupFromShaders();
		planeColors[i] = Vector4{ 1.0f,1.0f,1.0f,1.0f };
		planeTransforms[i].scale = { 0.5f,6.0f,0.5f };
		planeTransforms[i].translate = { 0.0f, 0.0f, 0.0f };
		planeTransforms[i].rotate = { Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1), Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1), Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1) };
	}

	renderRings_.resize(10);
	ringColors.resize(10);
	ringTransforms.resize(10);
	for (int i = 0; i < 10; ++i)
	{
		renderRings_[i] = std::make_unique<RenderObject>();
		renderRings_[i]->modelID_ = Game::Resource::LoadModel("resources/Prototypes/model/ring.obj");
		renderRings_[i]->psoConfig_.ps = "resources/Shaders/SimpleModel/SimpleModel.PS.hlsl";
		renderRings_[i]->psoConfig_.vs = "resources/Shaders/SimpleModel/SimpleModel.VS.hlsl";
		renderRings_[i]->psoConfig_.blendID = BlendStateID::Add;
		renderRings_[i]->psoConfig_.depthStencilID = DepthStencilID::TestOnly;
		renderRings_[i]->SetupFromShaders();
		ringColors[i] = Vector4{ 1.0f,1.0f,1.0f,1.0f };
		ringTransforms[i].scale = { 6.0f,6.0f,6.0f };
		ringTransforms[i].translate = { 0.0f, 0.0f, 0.0f };
		ringTransforms[i].rotate = { Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1), Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1), Game::Math::Rand::RandFloat(-3.14f, 3.141f, 1) };
	}
}

TestParticle::~TestParticle()
{}

void TestParticle::Initialize()
{}

void TestParticle::Update()
{
	Matrix4x4 viewProjection = Game::Camera::Getter::GetCurrentViewProjectionMatrix();
	for (int i = 0; i < 10; ++i)
	{
		planeColors[i].w -= 0.02f;
		if (Game::Math::Rand::RandInt(0, 50) == i)
		{
			planeColors[i].w = 1.0f;
		}

		Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(planeTransforms[i].scale, planeTransforms[i].rotate, planeTransforms[i].translate);
		Matrix4x4 worldViewProjection = worldMatrix * viewProjection;
		renderPlanes_[i]->SetCBufferData(0, ShaderType::PixelShader, &planeColors[i]);
		renderPlanes_[i]->SetCBufferData(1, ShaderType::PixelShader, &t_uvChecker);
		renderPlanes_[i]->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
		renderPlanes_[i]->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
	}

	for (int i = 0; i < 10; ++i)
	{
		ringColors[i].w -= 0.02f;
		if (Game::Math::Rand::RandInt(0, 50) == i)
		{
			ringColors[i].w = 1.0f;
		}
		Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(ringTransforms[i].scale, ringTransforms[i].rotate, ringTransforms[i].translate);
		Matrix4x4 worldViewProjection = worldMatrix * viewProjection;
		renderRings_[i]->SetCBufferData(0, ShaderType::PixelShader, &ringColors[i]);
		renderRings_[i]->SetCBufferData(1, ShaderType::PixelShader, &t_gradationLine);
		renderRings_[i]->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
		renderRings_[i]->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
	}
}

void TestParticle::Draw()
{
	uint32_t rtID = Game::Resource::GetRenderTextureID("Main");

	for (int i = 0; i < 10; ++i)
	{
		renderPlanes_[i]->Draw(rtID);
	}
	for (int i = 0; i < 10; ++i)
	{
		renderRings_[i]->Draw(rtID);
	}
}

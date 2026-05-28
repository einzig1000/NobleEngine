#include "TestParticle.h"
#include <Game.h>
#include <numbers>

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

	renderCylinder_ = std::make_unique<RenderObject>();
	renderCylinder_->modelID_ = Game::Resource::LoadModel("resources/Prototypes/model/cylinder.obj");
	renderCylinder_->psoConfig_.ps = "resources/Shaders/SimpleModel/SimpleModel.PS.hlsl";
	renderCylinder_->psoConfig_.vs = "resources/Shaders/SimpleModel/SimpleModel.VS.hlsl";
	renderCylinder_->psoConfig_.blendID = BlendStateID::Add;
	renderCylinder_->psoConfig_.depthStencilID = DepthStencilID::TestOnly;
	renderCylinder_->SetupFromShaders();
	cylinderColor = Vector4{ 0.1f,1.0f,1.0f,1.0f };
	cylinderTransform.scale = { 1.0f,1.0f,1.0f };
	cylinderTransform.translate = { 0.0f, 0.0f, 0.0f };
	cylinderTransform.rotate = { 0.0f, 0.0f, 3.141592f };


	const uint32_t kRingDivide = 32;
	const float kOuterRadius = 1.0f;
	const float kInnerRadius = 0.2f;
	const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kRingDivide);

	std::vector<VertexData> vertexData;

	for (uint32_t index = 0; index < kRingDivide; ++index)
	{
		float sin = std::sin(index * radianPerDivide);
		float cos = std::cos(index * radianPerDivide);
		float sinNext = std::sin((index + 1) * radianPerDivide);
		float cosNext = std::cos((index + 1) * radianPerDivide);
		float u = float(index) / float(kRingDivide);
		float uNext = float(index + 1) / float(kRingDivide);

		// 法線はXY平面のリングなので +Z 固定
		Vector3 normal = { 0.0f, 0.0f, 1.0f };

		// 4頂点
		VertexData v1{ { -sin * kOuterRadius,     cos * kOuterRadius,     0.0f, 1.0f }, { u,     0.0f }, normal };
		VertexData v2{ { -sinNext * kOuterRadius, cosNext * kOuterRadius, 0.0f, 1.0f }, { uNext, 0.0f }, normal };
		VertexData v3{ { -sin * kInnerRadius,     cos * kInnerRadius,     0.0f, 1.0f }, { u,     1.0f }, normal };
		VertexData v4{ { -sinNext * kInnerRadius, cosNext * kInnerRadius, 0.0f, 1.0f }, { uNext, 1.0f }, normal };

		// 三角形①: v1, v2, v3
		vertexData.push_back(v1);
		vertexData.push_back(v2);
		vertexData.push_back(v3);

		// 三角形②: v3, v2, v4
		vertexData.push_back(v3);
		vertexData.push_back(v2);
		vertexData.push_back(v4);
	}

	m_ring_ = Game::Resource::CreateModel(vertexData);

	renderRings_.resize(10);
	ringColors.resize(10);
	ringTransforms.resize(10);
	for (int i = 0; i < 10; ++i)
	{
		renderRings_[i] = std::make_unique<RenderObject>();
		renderRings_[i]->modelID_ = m_ring_;
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

	{
		cylinderTransform.rotate.y += 0.002f;
		Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(cylinderTransform.scale, cylinderTransform.rotate, cylinderTransform.translate);
		Matrix4x4 worldViewProjection = worldMatrix * viewProjection;
		renderCylinder_->SetCBufferData(0, ShaderType::PixelShader, &cylinderColor);
		renderCylinder_->SetCBufferData(1, ShaderType::PixelShader, &t_gradationLine);
		renderCylinder_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
		renderCylinder_->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
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
	//renderCylinder_->Draw(rtID);
}

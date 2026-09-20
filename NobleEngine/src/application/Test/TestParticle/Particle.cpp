#include "Particle.h"

Particle::Particle()
{
	emitterSphere.count = 10;
	emitterSphere.frequency = 0.5f;
	emitterSphere.frequencyTime = 0.0f;
	emitterSphere.translate = { 0.0f, 0.0f, 0.0f };
	emitterSphere.radius = 0.1f;
	emitterSphere.emit = 0;

	particleSRVID_ = Game::Resource::CreateCompute(sizeof(ParticleData), 1024);
	freeListIndexSRVID_ = Game::Resource::CreateCompute(sizeof(int32_t), 1024);
	freeListSRVID_ = Game::Resource::CreateCompute(sizeof(int32_t), 1024);

	render_ = std::make_unique<RenderObject>();
	render_->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	render_->psoConfig_.ps = "assets/shaders/Particle/Particle.PS.hlsl";
	render_->psoConfig_.vs = "assets/shaders/Particle/Particle.VS.hlsl";
	render_->SetupFromShaders();
	//render_->psoConfig_.blendID = BlendStateID::Add;
	//render_->psoConfig_.depthStencilID = DepthStencilID::TestOnly;
	render_->instanceNum_ = 1024;

	initializeCompute_ = std::make_unique<ComputeObject>();
	initializeCompute_->psoConfig_.cs = "assets/shaders/Particle/InitializeParticle.CS.hlsl";
	initializeCompute_->SetupFromShaders();
	initializeCompute_->SetURegisterData(0, Game::Resource::GetUAV(particleSRVID_));
	initializeCompute_->SetURegisterData(1, Game::Resource::GetUAV(freeListIndexSRVID_));
	initializeCompute_->SetURegisterData(2, Game::Resource::GetUAV(freeListSRVID_));
	initializeCompute_->RegisterOutput(particleSRVID_);
	initializeCompute_->RegisterOutput(freeListIndexSRVID_);
	initializeCompute_->RegisterOutput(freeListSRVID_);
	initializeCompute_->Dispatch();

	emitCompute_ = std::make_unique<ComputeObject>();
	emitCompute_->psoConfig_.cs = "assets/shaders/Particle/EmitParticle.CS.hlsl";
	emitCompute_->SetupFromShaders();
	emitCompute_->RegisterOutput(particleSRVID_);
	emitCompute_->RegisterOutput(freeListIndexSRVID_);
	emitCompute_->RegisterOutput(freeListSRVID_);

	updateCompute_ = std::make_unique<ComputeObject>();
	updateCompute_->psoConfig_.cs = "assets/shaders/Particle/UpdateParticle.CS.hlsl";
	updateCompute_->SetupFromShaders();
	updateCompute_->RegisterOutput(particleSRVID_);
	updateCompute_->RegisterOutput(freeListIndexSRVID_);
	updateCompute_->RegisterOutput(freeListSRVID_);
}

Particle::~Particle()
{}

void Particle::Initialize()
{}

void Particle::Update(int32_t cameraID)
{
	Matrix4x4 viewProjection = Game::Camera::Getter::GetViewProjectionMatrix(cameraID);

	float deltaTime = Game::Time::GetScaledDeltaTimeMs() * 0.001f;
	emitterSphere.frequencyTime += deltaTime;
	if (emitterSphere.frequency <= emitterSphere.frequencyTime)
	{
		emitterSphere.frequencyTime -= emitterSphere.frequency;
		emitterSphere.emit = 1;
	}
	else
	{
		emitterSphere.emit = 0;
	}

	emitCompute_->SetURegisterData(0, Game::Resource::GetUAV(particleSRVID_));
	emitCompute_->SetURegisterData(1, Game::Resource::GetUAV(freeListIndexSRVID_));
	emitCompute_->SetURegisterData(2, Game::Resource::GetUAV(freeListSRVID_));
	emitCompute_->SetBRegisterData(0, &emitterSphere);
	Vector3 rand = { Game::Math::Rand::RandFloat(-1.0f, 1.0f, 1), Game::Math::Rand::RandFloat(-1.0f, 1.0f, 1), Game::Math::Rand::RandFloat(-1.0f, 1.0f, 1) };
	emitCompute_->SetBRegisterData(1, &rand);

	//updateCompute_->size.x = 1024;
	updateCompute_->SetURegisterData(0, Game::Resource::GetUAV(particleSRVID_));
	updateCompute_->SetURegisterData(1, Game::Resource::GetUAV(freeListIndexSRVID_));
	updateCompute_->SetURegisterData(2, Game::Resource::GetUAV(freeListSRVID_));
	updateCompute_->SetBRegisterData(0, &deltaTime);

	TransformationMatrix perView;
	perView.WVP = Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
	perView.World = Game::Camera::Getter::GetBillboardMatrix(cameraID);
	render_->SetTRegisterData(0, ShaderType::VertexShader, Game::Resource::GetSRV(particleSRVID_));
	render_->SetBRegisterData(0, ShaderType::VertexShader, &perView);
}

void Particle::Draw()
{
	emitCompute_->Dispatch();
	updateCompute_->Dispatch();
	render_->Draw();
}

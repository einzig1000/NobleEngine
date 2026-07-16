#include "TestAnimation.h"
#include <Game.h>

TestAnimation::TestAnimation()
{
	render_ = std::make_unique<RenderObject>();
	render_->modelID_ = Game::Asset::Model::Load("resources/prototypes/model/human/sneakWalk.gltf");
	render_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	//render_->psoConfig_.vs = "resources/shaders/Skinning/Skinning.VS.hlsl";
	render_->psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModelNonIASet.VS.hlsl";
	render_->SetupFromShaders();

	compute_ = std::make_unique<ComputeObject>();
	compute_->psoConfig_.cs = "resources/shaders/Skinning/Skinning.CS.hlsl";
	compute_->SetupFromShaders();

	// アニメーションデータ
	animationID_ = Game::Asset::Animation::Load("resources/prototypes/model/human/sneakWalk.gltf", "sneakWalk");
	// テクスチャデータ
	texID_ = Game::Asset::Texture::Load("resources/prototypes/texture/AnimatedCube_BaseColor.png");
	// 動的SRVの作成
	WellSRVID_ = Game::Resource::CreateDynamic();
	vertexSRVID_ = Game::Resource::CreateDynamic();
	vertexInflenceSRVID_ = Game::Resource::CreateDynamic();

	ModelData* modelData = nullptr;
	modelData = Game::Asset::Model::GetData(render_->modelID_);
	vertices = modelData->vertices;
	skeleton_ = modelData->skeleton;
	skinCluster_ = modelData->skinCluster;
	vertexInfluences.resize(modelData->skinCluster.mappedInfluences.size());
	for (int32_t i = 0; i < modelData->skinCluster.mappedInfluences.size(); i++)
	{
		vertexInfluences[i] = modelData->skinCluster.mappedInfluences[i];
	}



	resultVertices = modelData->vertices;
	resultSRVID_ = Game::Resource::CreateCompute(sizeof(VertexData), size_t(resultVertices.size()));

	compute_->size.x = resultVertices.size();

	compute_->RegisterOutput(resultSRVID_);
}

TestAnimation::~TestAnimation()
{}

void TestAnimation::Initialize()
{}

void TestAnimation::Update(int32_t cameraID)
{
	animationTime_ += Game::Time::GetDeltaTime();

	Matrix4x4 viewProjection = Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
	Vector4 color = Vector4{ 1.0f,1.0f,1.0f,1.0f };

	Game::Asset::Animation::ComputeAnimationData(animationID_, skeleton_, skinCluster_, animationTime_);

	Matrix4x4 animationMatrix = Matrix4x4::MakeIdentity4x4();
	Matrix4x4 worldViewProjection = animationMatrix * viewProjection;

	Game::Resource::UpdateData(WellSRVID_, skinCluster_.mappedPalette.data(), sizeof(WellForGPU), skinCluster_.mappedPalette.size());
	Game::Resource::UpdateData(vertexSRVID_, vertices.data(), sizeof(VertexData), vertices.size());
	Game::Resource::UpdateData(vertexInflenceSRVID_, vertexInfluences.data(), sizeof(VertexInfluence), vertexInfluences.size());

	render_->SetCBufferData(0, ShaderType::PixelShader, &color);
	render_->SetCBufferData(1, ShaderType::PixelShader, &texID_);
	render_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
	render_->SetCBufferData(1, ShaderType::VertexShader, &animationMatrix);
	render_->SetSBufferData(0, ShaderType::VertexShader, Game::Resource::GetSRV(resultSRVID_));

	compute_->SetSBufferData(0, Game::Resource::GetSRV(WellSRVID_));
	compute_->SetSBufferData(1, Game::Resource::GetSRV(vertexSRVID_));
	compute_->SetSBufferData(2, Game::Resource::GetSRV(vertexInflenceSRVID_));
	compute_->SetUAVData(0, Game::Resource::GetUAV(resultSRVID_));
	uint32_t numVertices = static_cast<uint32_t>(resultVertices.size());
	compute_->SetCBufferData(0, &numVertices);
}

void TestAnimation::Draw(int32_t renderTextureID)
{
	render_->Draw(renderTextureID);
	compute_->Dispatch();
}

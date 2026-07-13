//#include "TestAnimation.h"
//#include <Game.h>
//
//TestAnimation::TestAnimation()
//{
//	render_ = std::make_unique<RenderObject>();
//	render_->modelID_ = Game::Asset::Model::Load("resources/prototypes/model/human/sneakWalk.gltf");
//	render_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
//	render_->psoConfig_.vs = "resources/shaders/Skinning/Skinning.VS.hlsl";
//	render_->SetupFromShaders();
//
//	// アニメーションデータ
//	animationID_ = Game::Asset::Animation::Load("resources/prototypes/model/human/sneakWalk.gltf", "sneakWalk");
//	// テクスチャデータ
//	texID_ = Game::Asset::Texture::Load("resources/prototypes/texture/AnimatedCube_BaseColor.png");
//
//	ModelData* modelData = nullptr;
//	modelData = Game::Asset::Model::GetData(render_->modelID_);
//	skeleton = modelData->skeleton;
//	skinCluster_ = modelData->skinCluster;
//}
//
//TestAnimation::~TestAnimation()
//{}
//
//void TestAnimation::Initialize()
//{}
//
//void TestAnimation::Update(int32_t cameraID)
//{
//	animationTime_ += Game::Time::GetDeltaTime();
//
//	Matrix4x4 viewProjection = Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
//	Vector4 color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
//
//	Game::Asset::Animation::ComputeAnimationData(animationID_, skeleton, skinCluster_, animationTime_);
//
//	Matrix4x4 animationMatrix = Matrix4x4::MakeIdentity4x4();
//	Matrix4x4 worldViewProjection = animationMatrix * viewProjection;
//
//	render_->SetCBufferData(0, ShaderType::PixelShader, &color);
//	render_->SetCBufferData(1, ShaderType::PixelShader, &texID_);
//	render_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
//	render_->SetCBufferData(1, ShaderType::VertexShader, &animationMatrix);
//	render_->SetSBufferData(0, ShaderType::VertexShader, skinCluster_.mappedPalette.data(), sizeof(WellForGPU), skinCluster_.mappedPalette.size());
//}
//
//void TestAnimation::Draw(int32_t renderTextureID)
//{
//	render_->Draw(renderTextureID);
//}
//

#include "TestAnimation.h"
#include <Game.h>

TestAnimation::TestAnimation()
{
	render_ = std::make_unique<RenderObject>();
	render_->modelID_ = Game::Asset::Model::Load("resources/prototypes/model/human/sneakWalk.gltf");
	render_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	render_->psoConfig_.vs = "resources/shaders/Skinning/Skinning.VS.hlsl";
	render_->SetupFromShaders();

	// アニメーションデータ
	animationID_ = Game::Asset::Animation::Load("resources/prototypes/model/human/sneakWalk.gltf", "sneakWalk");
	// テクスチャデータ
	texID_ = Game::Asset::Texture::Load("resources/prototypes/texture/AnimatedCube_BaseColor.png");
	// 動的SRVの作成
	heapIndex_ = Game::Resource::CreateDynamic();

	ModelData* modelData = nullptr;
	modelData = Game::Asset::Model::GetData(render_->modelID_);
	skeleton = modelData->skeleton;
	skinCluster_ = modelData->skinCluster;
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

	Game::Asset::Animation::ComputeAnimationData(animationID_, skeleton, skinCluster_, animationTime_);

	Matrix4x4 animationMatrix = Matrix4x4::MakeIdentity4x4();
	Matrix4x4 worldViewProjection = animationMatrix * viewProjection;

	render_->SetCBufferData(0, ShaderType::PixelShader, &color);
	render_->SetCBufferData(1, ShaderType::PixelShader, &texID_);
	render_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
	render_->SetCBufferData(1, ShaderType::VertexShader, &animationMatrix);
	Game::Resource::UpdateData(heapIndex_, skinCluster_.mappedPalette.data(), sizeof(WellForGPU), skinCluster_.mappedPalette.size());
	render_->SetSBufferData(0, ShaderType::VertexShader, Game::Resource::GetSRV(heapIndex_));
}

void TestAnimation::Draw(int32_t renderTextureID)
{
	render_->Draw(renderTextureID);
}

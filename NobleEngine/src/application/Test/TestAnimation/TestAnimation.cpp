#include "TestAnimation.h"
#include <Utilities/functions.h>
#include <Game.h>

TestAnimation::TestAnimation()
{
	render_ = std::make_unique<RenderObject>();
	render_->modelID_ = Game::Asset::Model::Load("assets/engine/model/human/sneakWalk.gltf");
	render_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	//render_->psoConfig_.vs = "assets/shaders/Skinning/Skinning.VS.hlsl";
	render_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModelNonIASet.VS.hlsl";
	render_->SetupFromShaders();

	compute_ = std::make_unique<ComputeObject>();
	compute_->psoConfig_.cs = "assets/shaders/Skinning/Skinning.CS.hlsl";
	compute_->SetupFromShaders();

	// アニメーションデータ
	animationID_ = Game::Asset::Animation::Load("assets/engine/model/human/sneakWalk.gltf", "sneakWalk");
	// テクスチャデータ
	texID_ = Game::Asset::Texture::Load("assets/engine/texture/AnimatedCube_BaseColor.png");
	// 動的SRVの作成
	WellSRVID_ = Game::Resource::CreateDynamic();
	vertexSRVID_ = Game::Resource::CreateDynamic();
	vertexInflenceSRVID_ = Game::Resource::CreateDynamic();

	const ModelData* modelData = Game::Asset::Model::GetData(render_->modelID_);
	vertices = modelData->vertices;
	skeleton_ = modelData->skeleton;
	skinCluster_ = modelData->skinCluster;
	vertexInfluences.resize(modelData->skinCluster.mappedInfluences.size());
	for (int32_t i = 0; i < modelData->skinCluster.mappedInfluences.size(); i++)
	{
		vertexInfluences[i] = modelData->skinCluster.mappedInfluences[i];
	}

	resultSRVID_ = Game::Resource::CreateCompute(sizeof(VertexData), size_t(modelData->vertices.size()));

	compute_->size.x = int32_t(modelData->vertices.size());

	compute_->RegisterOutput(resultSRVID_);
}

TestAnimation::~TestAnimation()
{}

void TestAnimation::Initialize()
{}

void TestAnimation::Update(int32_t cameraID)
{
	animationTime_ += Game::Time::GetScaledDeltaTimeMs();
	//animationTime_ = 0.5f;

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
	uint32_t numVertices = static_cast<uint32_t>(vertices.size());
	compute_->SetCBufferData(0, &numVertices);
}

void TestAnimation::Draw(int32_t renderTextureID)
{
	render_->Draw(renderTextureID);
	compute_->Dispatch();
}

Vector3 TestAnimation::GetVetexPos(int32_t index)
{
	const VertexInfluence& influence = vertexInfluences[index];
	const Vector4& localPosition = vertices[index].position;

	Vector4 skinnedPosition = { 0.0f, 0.0f, 0.0f, 0.0f };

	for (int32_t i = 0; i < 4; ++i)
	{
		if (influence.weights[i] > 0.0f)
		{
			const Matrix4x4& skeletonSpaceMatrix = skinCluster_.mappedPalette[influence.jointIndices[i]].skeletonSpaceMatrix;
			Vector4 transformed = Transform(localPosition, skeletonSpaceMatrix);

			skinnedPosition.x += transformed.x * influence.weights[i];
			skinnedPosition.y += transformed.y * influence.weights[i];
			skinnedPosition.z += transformed.z * influence.weights[i];
			skinnedPosition.w += transformed.w * influence.weights[i];
		}
	}

	// animationMatrix_ が現在Identityのため、スケルトン空間 = ワールド空間になっている
	return Vector3{ skinnedPosition.x, skinnedPosition.y, skinnedPosition.z };
}

int32_t TestAnimation::GetVertexIndexByJointName(const std::string& jointName)
{
	auto it = skeleton_.jointIndexByName.find(jointName);
	if (it == skeleton_.jointIndexByName.end())
	{
		return -1; // 該当するジョイントが見つからない
	}
	int32_t jointIndex = it->second;

	int32_t bestVertexIndex = -1;
	float bestWeight = 0.0f;

	for (size_t vertexIndex = 0; vertexIndex < vertexInfluences.size(); ++vertexIndex)
	{
		const VertexInfluence& influence = vertexInfluences[vertexIndex];
		for (int32_t i = 0; i < 4; ++i)
		{
			if (influence.jointIndices[i] == jointIndex && influence.weights[i] > bestWeight)
			{
				bestWeight = influence.weights[i];
				bestVertexIndex = int32_t(vertexIndex);
			}
		}
	}

	return bestVertexIndex; // 見つからなければ -1
}

Matrix4x4 TestAnimation::GetJointMatrixByName(const std::string& jointName)
{
	auto it = skeleton_.jointIndexByName.find(jointName);
	if (it == skeleton_.jointIndexByName.end())
	{
		return Matrix4x4::MakeIdentity4x4(); // 見つからなければ単位行列
	}
	int32_t jointIndex = it->second;

	// animationMatrix_ が現在Identityのため、スケルトン空間 = ワールド空間になっている
	return skeleton_.joints[jointIndex].skeletonSpaceMatrix;
}
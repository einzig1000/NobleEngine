#include "TestAnimation.h"
#include <Game.h>

namespace
{
	Vector3 CalculateValue(float time, const AnimationCurve<Vector3>& curve)
	{
		assert(!curve.keyFrames.empty());

		if (curve.keyFrames.size() == 1 || time <= curve.keyFrames[0].time)
			return curve.keyFrames[0].value;

		for (size_t i = 0; i < curve.keyFrames.size() - 1; ++i)
		{
			const auto& k0 = curve.keyFrames[i];
			const auto& k1 = curve.keyFrames[i + 1];

			if (k0.time <= time && time <= k1.time)
			{
				float t = (time - k0.time) / (k1.time - k0.time);
				return k0.value * (1.0f - t) + k1.value * t;
			}
		}

		return curve.keyFrames.back().value;
	}

	Quaternion CalculateValue(float time, const AnimationCurve<Quaternion>& curve)
	{
		assert(!curve.keyFrames.empty());

		if (curve.keyFrames.size() == 1 || time <= curve.keyFrames[0].time)
			return curve.keyFrames[0].value;

		for (size_t i = 0; i < curve.keyFrames.size() - 1; ++i)
		{
			const auto& k0 = curve.keyFrames[i];
			const auto& k1 = curve.keyFrames[i + 1];

			if (k0.time <= time && time <= k1.time)
			{
				float t = (time - k0.time) / (k1.time - k0.time);
				return Quaternion::Slerp(k0.value, k1.value, t);
			}
		}

		return curve.keyFrames.back().value;
	}
}


TestAnimation::TestAnimation()
{
	render_ = std::make_unique<RenderObject>();
	render_->modelID_ = Game::Resource::LoadModel("resources/Prototypes/model/AnimatedCube.gltf");
	render_->psoConfig_.ps = "resources/Shaders/SimpleModel/SimpleModel.PS.hlsl";
	render_->psoConfig_.vs = "resources/Shaders/SimpleModel/SimpleModel.VS.hlsl";
	render_->SetupFromShaders();

	animation = animationManager_.LoadAnimation("resources/Prototypes/model/AnimatedCube.gltf");
	tex = Game::Resource::LoadTexture("resources/Prototypes/texture/AnimatedCube_BaseColor.png");
	nodeAnimation = &animation.nodeAnimations["AnimatedCube"];
}

TestAnimation::~TestAnimation()
{}

void TestAnimation::Initialize()
{}

void TestAnimation::Update(float deltaTime)
{
	animationTime_ += deltaTime;
	animationTime_ = fmod(animationTime_, animation.duration);

	Matrix4x4 viewProjection = Game::Camera::Getter::GetCurrentViewProjectionMatrix();

	Vector4 color = Vector4{ 1.0f,1.0f,1.0f,1.0f };

	Vector3 translate = CalculateValue(animationTime_, nodeAnimation->translate);
	Quaternion rotate = CalculateValue(animationTime_, nodeAnimation->rotate);
	Vector3 scale = CalculateValue(animationTime_, nodeAnimation->scale);
	Matrix4x4 animationMatrix = Matrix4x4::MakeAffineMatrix(scale, rotate, translate);
	Matrix4x4 worldViewProjection = animationMatrix * viewProjection;

	render_->SetCBufferData(0, ShaderType::PixelShader, &color);
	render_->SetCBufferData(1, ShaderType::PixelShader, &tex);
	render_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
	render_->SetCBufferData(1, ShaderType::VertexShader, &animationMatrix);
}

void TestAnimation::Draw()
{
	uint32_t rtID = Game::Resource::GetRenderTextureID("Main");

	render_->Draw(rtID);
}

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
	render_->modelID_ = Game::Resource::Model::Load("resources/prototypes/model/human/sneakWalk.gltf");
	render_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	render_->psoConfig_.vs = "resources/shaders/Skinning/Skinning.VS.hlsl";
	//render_->psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModel.VS.hlsl";
	render_->SetupFromShaders();

	// 頂点データ
	modelData_ = Game::Resource::Model::GetData(render_->modelID_);
	// アニメーションデータ
	animation = animationManager_.LoadAnimation("resources/prototypes/model/human/sneakWalk.gltf");
	// テクスチャデータ
	tex = Game::Resource::Texture::Load("resources/prototypes/texture/AnimatedCube_BaseColor.png");

	//nodeAnimation = &animation.nodeAnimations["AnimatedCube"];
	skeleton = modelData_->skeleton;
	skinCluster_ = modelData_->skinCluster;
}

TestAnimation::~TestAnimation()
{}

void TestAnimation::Initialize()
{}

void TestAnimation::Update(float deltaTime)
{
	animationTime_ += deltaTime;
	animationTime_ = fmod(animationTime_, animation.duration);

	Matrix4x4 viewProjection = Game::Camera::Getter::GetViewProjectionMatrix(0);

	Vector4 color = Vector4{ 1.0f,1.0f,1.0f,1.0f };

	animationManager_.TestApplyAnimation(skeleton, animation, animationTime_);
	animationManager_.TestUpdateSkeleton(skeleton);
	animationManager_.TestUpdateSkinCluster(skeleton, skinCluster_);

	Matrix4x4 animationMatrix = Matrix4x4::MakeIdentity4x4();
	Matrix4x4 worldViewProjection = animationMatrix * viewProjection;

	render_->SetCBufferData(0, ShaderType::PixelShader, &color);
	render_->SetCBufferData(1, ShaderType::PixelShader, &tex);
	render_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
	render_->SetCBufferData(1, ShaderType::VertexShader, &animationMatrix);
	render_->SetSBufferData(0, ShaderType::VertexShader, skinCluster_.mappedPalette.data(), sizeof(WellForGPU), skinCluster_.mappedPalette.size());

	//render_->SetCBufferData(1, ShaderType::VertexShader, &animationMatrix);
}

void TestAnimation::Draw(int32_t renderTextureID)
{
	render_->Draw(renderTextureID);

	ImGui::Begin("Animation Info");
	ImGui::Text("Animation Time: %.2f / %.2f", animationTime_, animation.duration);
	ImGui::Text("Joint Count: %d", static_cast<int>(skeleton.joints.size()));
	ImGui::Text("Root Joint: %s", skeleton.joints[skeleton.root].name.c_str());

	ImGui::SeparatorText("Skeleton Info");
	for (const auto& joint : skeleton.joints)
	{
		ImGui::Text("Joint[%d] %s", joint.index, joint.name.c_str());
		ImGui::Text("Parent: %d", joint.parentIndex.value_or(-1));
		ImGui::Text("Children: ");
		for (auto c : joint.childrenIndex) ImGui::SameLine(), ImGui::Text("%d", c);
		ImGui::Text("LocalMatrix:");
		for (int r = 0; r < 4; r++)
			ImGui::Text("%f %f %f %f",
				joint.localMatrix.m[r][0], joint.localMatrix.m[r][1], joint.localMatrix.m[r][2], joint.localMatrix.m[r][3]);
		ImGui::Text("SkeletonSpace:");
		for (int r = 0; r < 4; r++)
			ImGui::Text("%f %f %f %f",
				joint.skeletonSpaceMatrix.m[r][0], joint.skeletonSpaceMatrix.m[r][1], joint.skeletonSpaceMatrix.m[r][2], joint.skeletonSpaceMatrix.m[r][3]);
	}


	ImGui::End();
}

#include "TestAnimation.h"
#include <Game.h>

TestAnimation::TestAnimation()
{
	render_ = std::make_unique<RenderObject>();
	render_->modelID_ = Game::Asset::Model::Load("resources/prototypes/model/human/sneakWalk.gltf");
	render_->psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	render_->psoConfig_.vs = "resources/shaders/Skinning/Skinning.VS.hlsl";
	render_->SetupFromShaders();

	// 頂点データ
	modelData_ = Game::Asset::Model::GetData(render_->modelID_);
	// アニメーションデータ
	animationID_ = Game::Asset::Animation::Load("resources/prototypes/model/human/sneakWalk.gltf", "sneakWalk");
	// テクスチャデータ
	texID_ = Game::Asset::Texture::Load("resources/prototypes/texture/AnimatedCube_BaseColor.png");

	skeleton = modelData_->skeleton;
	skinCluster_ = modelData_->skinCluster;
}

TestAnimation::~TestAnimation()
{}

void TestAnimation::Initialize()
{}

void TestAnimation::Update(int32_t cameraID)
{
	animationTime_ += Game::Time::GetDeltaTime();
	animationTime_ = fmod(animationTime_, animation.duration);

	Matrix4x4 viewProjection = Game::Camera::Getter::GetViewProjectionMatrix(cameraID);

	Vector4 color = Vector4{ 1.0f,1.0f,1.0f,1.0f };

	animationManager_.TestApplyAnimation(skeleton, animation, animationTime_);
	animationManager_.TestUpdateSkeleton(skeleton);
	animationManager_.TestUpdateSkinCluster(skeleton, skinCluster_);

	Matrix4x4 animationMatrix = Matrix4x4::MakeIdentity4x4();
	Matrix4x4 worldViewProjection = animationMatrix * viewProjection;

	render_->SetCBufferData(0, ShaderType::PixelShader, &color);
	render_->SetCBufferData(1, ShaderType::PixelShader, &texID_);
	render_->SetCBufferData(0, ShaderType::VertexShader, &worldViewProjection);
	render_->SetCBufferData(1, ShaderType::VertexShader, &animationMatrix);
	render_->SetSBufferData(0, ShaderType::VertexShader, skinCluster_.mappedPalette.data(), sizeof(WellForGPU), skinCluster_.mappedPalette.size());
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

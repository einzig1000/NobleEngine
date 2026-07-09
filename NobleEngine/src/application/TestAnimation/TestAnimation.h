#pragma once
#include <DrawSystem/RenderData/RenderObject.h>
#include <memory>

#include <AssetManager/Animation/AnimationManager.h>

class TestAnimation
{
public:
	TestAnimation();
	~TestAnimation();
	void Initialize();
	void Update(int32_t cameraID);
	void Draw(int32_t renderTextureID);

private:
	std::unique_ptr<RenderObject> render_;

	float animationTime_ = 0.0f;

	int32_t animationID_ = -1;

	ModelData* modelData_ = nullptr;


	AnimationData animation;
	NodeAnimation* nodeAnimation = nullptr;
	SkinCluster skinCluster_;
	Skeleton skeleton;
	uint32_t texID_ = -1;

};

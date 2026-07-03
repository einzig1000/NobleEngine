#pragma once
#include <DrawSystem/RenderData/RenderObject.h>
#include <memory>

#include <ResourceManager/Animation/AnimationManager.h>

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

	AnimationManager animationManager_;
	Animation animation;
	NodeAnimation* nodeAnimation = nullptr;
	ModelData* modelData_ = nullptr;
	SkinCluster skinCluster_;
	Skeleton skeleton;
	uint32_t tex;

};

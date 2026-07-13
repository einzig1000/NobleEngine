#pragma once
#include <DrawSystem/RenderData/RenderObject.h>
#include <definition/definition.h>
#include <memory>

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

	int32_t heapIndex_ = -1;
	int32_t animationID_ = -1;
	uint32_t texID_ = -1;
	SkinCluster skinCluster_;
	Skeleton skeleton;
};


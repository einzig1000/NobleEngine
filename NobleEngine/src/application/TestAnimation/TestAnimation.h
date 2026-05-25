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
	void Update(float deltaTime);
	void Draw();

private:
	std::unique_ptr<RenderObject> render_;

	float animationTime_ = 0.0f;

	AnimationManager animationManager_;
	Animation animation;
	NodeAnimation* nodeAnimation = nullptr;
	uint32_t tex;
};

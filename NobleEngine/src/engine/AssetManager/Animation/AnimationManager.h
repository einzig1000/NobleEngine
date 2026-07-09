#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include "AnimationBank/AnimationBank.h"
#include "AnimationLoader/AnimationLoader.h"
#include <memory>

class DirectXManager;

class AnimationManager
{
public:
	AnimationManager(DirectXManager* dxManager);
	~AnimationManager();
	
	AnimationBank* GetAnimationBank() const { return bank_.get(); }
	AnimationLoader* GetAnimationLoader() const { return loader_.get(); }

	// 1,骨ごとのlocal情報を更新し
	void TestApplyAnimation(Skeleton& skeleton, const AnimationData& animation, float time);

	// 2,骨ごとのlocal情報からSkeltonSpaceの情報を更新する
	void TestUpdateSkeleton(Skeleton& skeleton);

	// 3,SkeltonSpaceの情報からSkinClusterの情報を更新する
	void TestUpdateSkinCluster(const Skeleton& skeleton, SkinCluster& skinCluster);

private:
	std::unique_ptr<AnimationBank> bank_;
	std::unique_ptr<AnimationLoader> loader_;

};


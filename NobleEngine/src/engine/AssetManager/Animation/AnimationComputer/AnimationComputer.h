#pragma once
#include <EngineDefinition/EngineDefinition.h>

class AnimationBank;

class AnimationComputer
{
public:
	AnimationComputer(AnimationBank* bank);
	~AnimationComputer();

	void UpdateAnimation(int32_t animationID, Skeleton& skeleton, SkinCluster& skinCluster, float& time);

private:
	AnimationBank* bank_;

	// 1,骨ごとのlocal情報を更新し
	void ApplyAnimation(Skeleton& skeleton, const AnimationData& animation, float time);

	// 2,骨ごとのlocal情報からSkeltonSpaceの情報を更新する
	void UpdateSkeleton(Skeleton& skeleton);

	// 3,SkeltonSpaceの情報からSkinClusterの情報を更新する
	void UpdateSkinCluster(const Skeleton& skeleton, SkinCluster& skinCluster);
};


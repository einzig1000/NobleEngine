#pragma once
#include <EngineDefinition/EngineDefinition.h>

class AnimationBank;

class AnimationComputer
{
public:
	AnimationComputer(AnimationBank* bank);
	~AnimationComputer();

	void ComputeAnimationData(int32_t animationID, SkinInstance& skin, const SkinBindData& bind, float& time);
	
	// skeleton(スキン)を介さず、親を辿りながら合成した1つの行列を返す
	// 剛体アイテムのスイングなど、ボーンを持たない多段階層のアニメーションに使う
	Matrix4x4 SampleNodeHierarchy(int32_t animationID, const std::string& nodeName, float& time);

private:

	AnimationBank* bank_;

	// 0,skin.boundChannelsをanimationIDに合わせて作り直す(animationIDが変わった時だけ呼ばれる)
	void BindChannels(SkinInstance& skin, const AnimationData& animation, int32_t animationID);

	// 1,骨ごとのlocal情報を更新し
	void ApplyAnimation(SkinInstance& skin, float time);

	// 2,骨ごとのlocal情報からSkeltonSpaceの情報を更新する
	void UpdateSkeleton(Skeleton& skeleton);

	// 3,SkeltonSpaceの情報からSkinClusterの情報を更新する
	void UpdatePalette(const Skeleton& skeleton, const SkinBindData& bind, std::vector<WellForGPU>& palette);

};


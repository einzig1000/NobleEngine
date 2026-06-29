#pragma once
#include <definition/definition.h>

class AnimationManager
{
public:
	AnimationManager();
	~AnimationManager();

	// アニメーション読み込み
	Animation LoadAnimation(const std::string& filePath);
	
	// データ取得
	Animation* GetAnimationData(int32_t animationID);
	
	// アニメーション数を取得
	size_t GetAnimationCount() const { return animations.size(); }

	// 1,骨ごとのlocal情報を更新し
	void TestApplyAnimation(Skeleton& skeleton, const Animation& animation, float time);

	// 2,骨ごとのlocal情報からSkeltonSpaceの情報を更新する
	void TestUpdateSkeleton(Skeleton& skeleton);

	// 3,SkeltonSpaceの情報からSkinClusterの情報を更新する
	void TestUpdateSkinCluster(const Skeleton& skeleton, SkinCluster& skinCluster);

private:

	std::unordered_map<std::string, Animation> animations;

	// アニメーションを読み込む関数
	Animation LoadAnimationFile(const std::string& filePath);
};


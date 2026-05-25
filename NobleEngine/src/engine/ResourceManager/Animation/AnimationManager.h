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
	
	// アニメーションリストを取得
	//std::vector<Animation>& GetAnimationList() { return animations; }

private:
	// アニメーションデータを詰める
	//std::vector<Animation> animations;
	//std::unordered_map<std::string, int32_t> animationPathToID;

	std::unordered_map<std::string, Animation> animations;

	// アニメーションを読み込む関数
	Animation LoadAnimationFile(const std::string& filePath);
};


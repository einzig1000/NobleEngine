#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <unordered_map>
#include <memory>

class AnimationBank
{
public:
	// アニメーションデータを追加
	int32_t AddAnimationData(const std::string& filePath, std::unique_ptr<AnimationData> animationData);
	// filePathが同じアニメーションデータが存在するか
	int32_t IsAnimationDataExist(const std::string filePath) const;
	// animationIDからアニメーションデータを取得
	const AnimationData* GetAnimationData(int32_t animationID) const;
	// アニメーションリストを取得
	const std::vector<std::unique_ptr<AnimationData>>& GetAnimationList() const { return dataList_; }

private:
	std::unordered_map<std::string, int32_t> pathToIDMap_;
	std::vector<std::unique_ptr<AnimationData>> dataList_;
};


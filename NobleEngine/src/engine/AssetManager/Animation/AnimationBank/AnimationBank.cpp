#include "AnimationBank.h"

int32_t AnimationBank::AddAnimationData(const std::string& filePath, std::unique_ptr<AnimationData> animationData)
{
	int32_t ID = static_cast<int32_t>(dataList_.size());

	pathToIDMap_[filePath] = ID;
	dataList_.push_back(std::move(animationData));

	return ID;
}

int32_t AnimationBank::IsAnimationDataExist(const std::string filePath) const
{
	auto it = pathToIDMap_.find(filePath);
	if (it != pathToIDMap_.end())
	{
		return it->second;
	}
	return -1;
}

const AnimationData* AnimationBank::GetAnimationData(int32_t animationID) const
{
	if (animationID >= 0 && animationID < static_cast<int32_t>(dataList_.size()))
	{
		return dataList_[animationID].get();
	}
	return nullptr;
}

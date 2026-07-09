#include "AnimationBank.h"

int32_t AnimationBank::AddAnimationData(const std::string& filePath, std::unique_ptr<AnimationData> animationData)
{
	int32_t ID = static_cast<int32_t>(objects_.size());

	pathToIDMap_[filePath] = ID;
	objects_.push_back(std::move(animationData));

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

AnimationData* AnimationBank::GetAnimationData(int32_t animationID)
{
	if (animationID >= 0 && animationID < static_cast<int32_t>(objects_.size()))
	{
		return objects_[animationID].get();
	}
	return nullptr;
}

#include "ModelBank.h"

int32_t ModelBank::AddModelData(const std::string& filePath, std::unique_ptr<ModelData> modelData)
{
	int32_t ID = static_cast<int32_t>(objects_.size());

	pathToIDMap_[filePath] = ID;
    objects_.push_back(std::move(modelData));
    return ID;
}

int32_t ModelBank::IsModelDataExist(const std::string filePath) const
{
	auto it = pathToIDMap_.find(filePath);
	if (it != pathToIDMap_.end())
	{
		return it->second;
	}
    return -1;
}

const ModelData* ModelBank::GetModelData(int32_t modelID) const
{
	if (modelID >= 0 && modelID < static_cast<int32_t>(objects_.size()))
	{
		return objects_[modelID].get();
	}
	return nullptr;
}

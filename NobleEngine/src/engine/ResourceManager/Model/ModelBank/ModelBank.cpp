#include "ModelBank.h"

void ModelBank::AddModelData(const std::string& filePath, const int32_t ID, std::unique_ptr<ModelData> modelData)
{
	pathToIDMap_[filePath] = ID;
    objects_.push_back(std::move(modelData));
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

ModelData* ModelBank::GetModelData(int32_t modelID)
{
	if (modelID >= 0 && modelID < static_cast<int32_t>(objects_.size()))
	{
		return objects_[modelID].get();
	}
	return nullptr;
}

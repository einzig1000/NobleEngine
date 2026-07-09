#include "ItemConfig.h"
#include <Utilities/Json/JsonManager.h>

ItemConfig::ItemConfig()
{
	//JsonManager::Load("resources/ItemConfig.json");
	//JsonManager::Load("resources/BlockConfig.json");

	int32_t BlockMax = 0;
	JsonManager::Load("resources/BlockConfig.json", "MAX", BlockMax);

	for (int32_t i = 0; i < BlockMax; i++)
	{
		BlockInfo info;
		JsonManager::Load("resources/BlockConfig.json", "color_" + std::to_string(i), info.color);
		JsonManager::Load("resources/BlockConfig.json", "durability_" + std::to_string(i), info.durability);
		JsonManager::Load("resources/BlockConfig.json", "isTransparent_" + std::to_string(i), info.isTransparent);
		std::string blockIDStr;
		JsonManager::Load("resources/BlockConfig.json", "blockID_" + std::to_string(i), blockIDStr);
		info.ID = StringToEnum<BlockID>(blockIDStr);
	}
}

ItemConfig::~ItemConfig()
{}

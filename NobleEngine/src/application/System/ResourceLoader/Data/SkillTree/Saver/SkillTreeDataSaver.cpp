#include "SkillTreeDataSaver.h"
#include <System/ResourceLoader/Data/SkillTree/Bank/SkillTreeDataBank.h>
#include <externals/MagicEnum/magic_enum.hpp>
#include <Utilities/Json/JsonManager.h>

namespace
{
	const std::string kPath = "assets/application/json/SkillTree.json";
}

SkillTreeDataSaver::SkillTreeDataSaver(SkillTreeDataBank* bank)
	: bank_(bank)
{}

SkillTreeDataSaver::~SkillTreeDataSaver()
{}

void SkillTreeDataSaver::Save(const std::vector<SkillNodeData>& nodes)
{
	bank_->SetNodes(nodes);

	// ルートごと空にしてから書く(ノードを減らしたときに古いのが残らないように)
	JsonManager::AddParam(kPath, "", nlohmann::json::object());

	JsonManager::AddParam(kPath, "/nodesSize", nodes.size());
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		const std::string key = "/nodes/" + std::to_string(i);
		const SkillNodeData& node = nodes[i];

		JsonManager::AddParam(kPath, key + "/id", node.id);
		JsonManager::AddParam(kPath, key + "/name", node.name);
		JsonManager::AddParam(kPath, key + "/description", node.description);
		JsonManager::AddParam(kPath, key + "/iconPath", node.iconPath);
		JsonManager::AddParam(kPath, key + "/position", node.position);
		JsonManager::AddParam(kPath, key + "/parents", node.parents);
		JsonManager::AddParam(kPath, key + "/effectType", std::string(magic_enum::enum_name(node.effectType)));
		JsonManager::AddParam(kPath, key + "/effectValue", node.effectValue);
		JsonManager::AddParam(kPath, key + "/levelCost", node.levelCost);

		JsonManager::AddParam(kPath, key + "/itemCosts", nlohmann::json::array());
		for (size_t k = 0; k < node.itemCosts.size(); ++k)
		{
			const std::string costKey = key + "/itemCosts/" + std::to_string(k);
			JsonManager::AddParam(kPath, costKey + "/id", std::string(magic_enum::enum_name(node.itemCosts[k].id)));
			JsonManager::AddParam(kPath, costKey + "/count", node.itemCosts[k].count);
		}
	}

	JsonManager::Save(kPath);
}
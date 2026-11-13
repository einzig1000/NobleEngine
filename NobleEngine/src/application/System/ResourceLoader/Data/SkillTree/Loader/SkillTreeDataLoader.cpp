#include "SkillTreeDataLoader.h"
#include <System/ResourceLoader/Data/SkillTree/Bank/SkillTreeDataBank.h>
#include <externals/MagicEnum/magic_enum.hpp>
#include <Utilities/Json/JsonManager.h>
#include <Utilities/Logger/Logger.h>
#include <Game.h>
#include <unordered_set>

namespace
{
	const std::string kPath = "assets/application/json/SkillTree.json";
}

SkillTreeDataLoader::SkillTreeDataLoader(SkillTreeDataBank* bank)
	: bank_(bank)
{
	Load();
}

SkillTreeDataLoader::~SkillTreeDataLoader()
{}

void SkillTreeDataLoader::Load()
{
	size_t nodesSize = 0;
	JsonManager::Load(kPath, "/nodesSize", nodesSize);

	std::vector<SkillNodeData> nodes(nodesSize);
	std::unordered_set<int32_t> usedIDs;
	for (size_t i = 0; i < nodesSize; ++i)
	{
		const std::string key = "/nodes/" + std::to_string(i);
		SkillNodeData& node = nodes[i];

		JsonManager::Load(kPath, key + "/id", node.id);
		JsonManager::Load(kPath, key + "/name", node.name);
		JsonManager::Load(kPath, key + "/description", node.description);
		JsonManager::Load(kPath, key + "/iconPath", node.iconPath);
		JsonManager::Load(kPath, key + "/position", node.position);
		JsonManager::Load(kPath, key + "/parents", node.parents);
		JsonManager::Load(kPath, key + "/effectValue", node.effectValue);
		JsonManager::Load(kPath, key + "/levelCost", node.levelCost);

		// 効果の種類は名前で保存している(enumの並びを変えても壊れないように)
		std::string effectName;
		JsonManager::Load(kPath, key + "/effectType", effectName);
		node.effectType = magic_enum::enum_cast<SkillEffectType>(effectName).value_or(SkillEffectType::None);
		if (!effectName.empty() && !magic_enum::enum_cast<SkillEffectType>(effectName).has_value())
		{
			Log("SkillTree.json: ノード%d の効果 %s がありません", node.id, effectName.c_str());
		}

		// 必要素材(無くなるまで読む)
		for (size_t k = 0;; ++k)
		{
			const std::string costKey = key + "/itemCosts/" + std::to_string(k);
			std::string itemName;
			if (!JsonManager::Load(kPath, costKey + "/id", itemName)) break;

			SkillCostItem cost;
			cost.id = magic_enum::enum_cast<ItemID>(itemName).value_or(ItemID::MAX);
			JsonManager::Load(kPath, costKey + "/count", cost.count);
			if (cost.id == ItemID::MAX)
			{
				Log("SkillTree.json: ノード%d の素材 %s がありません", node.id, itemName.c_str());
				continue;
			}
			node.itemCosts.push_back(cost);
		}

		// アイコン
		if (!node.iconPath.empty())
		{
			node.iconTextureID = Game::Asset::Texture::Load(node.iconPath);
		}

		// 番号の重複
		if (!usedIDs.insert(node.id).second)
		{
			Log("SkillTree.json: ノード番号 %d が重複しています", node.id);
		}
	}

	bank_->SetNodes(nodes);
}
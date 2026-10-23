#include "UIDataSaver.h"
#include <System/ResourceLoader/Data/UI/Bank/UIDataBank.h>
#include <externals/MagicEnum/magic_enum.hpp>
#include <Utilities/Json/JsonManager.h>

UIDataSaver::UIDataSaver(UIDataBank* bank)
	: bank_(bank)
{}

UIDataSaver::~UIDataSaver()
{}

void UIDataSaver::Save(UIElementType type, const std::vector<ElementData>& data)
{
	bank_->SetData(type, data);

	const std::string path = "assets/application/json/UIConfig.json";
	const std::string base = "/" + std::string(magic_enum::enum_name(type));

	JsonManager::AddParam(path, base, nlohmann::json::object());

	JsonManager::AddParam(path, base + "/elementsSize", data.size());
	for (size_t i = 0; i < data.size(); ++i)
	{
		const std::string key = base + "/elements/" + std::to_string(i);
		JsonManager::AddParam(path, key + "/label", data[i].label);
		JsonManager::AddParam(path, key + "/labelIndex", data[i].labelIndex);
		JsonManager::AddParam(path, key + "/texturePath", data[i].texturePath);
		JsonManager::AddParam(path, key + "/translateRate", data[i].translateRate);
		JsonManager::AddParam(path, key + "/pivot", data[i].pivot);
		JsonManager::AddParam(path, key + "/scale", data[i].scale);
		JsonManager::AddParam(path, key + "/rotate", data[i].rotate);
	}

	JsonManager::Save(path);
}
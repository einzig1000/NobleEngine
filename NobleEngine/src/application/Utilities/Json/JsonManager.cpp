#include <Utilities/Json/JsonManager.h>
#include <Utilities/functions.h>
#include <Utilities/Logger/Logger.h>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

std::unordered_map<std::string, nlohmann::json> JsonManager::dataMap;


bool JsonManager::Load(const std::string& path)
{
    std::ifstream ifs(path);
    if (!ifs.is_open())
    {
		Log("%s Json::LoadFailed パスが見つかりませんでした。\n", path.c_str());
        return false;
    }

    nlohmann::json j;
    ifs >> j;

    dataMap[path] = j;
    return true;
}

void JsonManager::LoadAll(const std::string& directoryPath)
{
    if (!std::filesystem::exists(directoryPath))
    {
        Log("%s Json::LoadAllFailed ディレクトリが存在しません。\n", directoryPath.c_str());
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
    {
        if (entry.path().extension() == ".json")
        {
            Load(entry.path().string());
        }
    }
}


bool JsonManager::Save(const std::string& path)
{
    auto it = dataMap.find(path);
    if (it == dataMap.end())
    {
		Log("%s Json::SaveFailed dataMapに該当データが見つかりませんでした。\n", path.c_str());
        return false;
    }

    std::filesystem::path p(path);
    std::filesystem::create_directories(p.parent_path());

    std::ofstream ofs(path);
    if (!ofs.is_open())
    {
		Log("%s Json::SaveFailed ファイルを開けませんでした。\n", path.c_str());
        return false;
    }

    ofs << it->second.dump(2);
	Log("%s Json::SaveSuccess\n", path.c_str());
    return true;
}

void JsonManager::SaveAll()
{
    for (auto& [path, _] : dataMap)
    {
        Save(path); 
    }
}

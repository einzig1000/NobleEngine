#pragma once
#include <externals/nlohmann/json.hpp>
#include "definition/definition.h"
#include <vector>
#include <string>

/// <summary>
/// JSON管理クラス
/// </summary>
class JsonManager
{
public:
	/// <summary>
	/// dataMapにパラメータを追加する
	/// </summary>
	/// <typeparam name="T"> 値の型 </typeparam>
	/// <param name="path"> ファイルパス </param>
	/// <param name="key"> キー </param>
	/// <param name="value"> 値 </param>
	template<typename T>
	static void AddParam(const std::string& path, const std::string& key, const T& value)
	{
		dataMap[path][key] = value;
	}

	/// <summary>
	/// dataMapからパラメータを取得する
	/// </summary>
	/// <typeparam name="T"> 値の型 </typeparam>
	/// <param name="path"> ファイルパス </param>
	/// <param name="key"> キー </param>
	/// <param name="outValue"> 取得した値の出力先 </param>
	/// <returns> 成否 </returns>
	template<typename T>
	static bool Load(const std::string& path, const std::string& key, T& outValue)
	{
		auto it = dataMap.find(path);
		if (it == dataMap.end()) return false;

		if (!it->second.contains(key)) return false;

		outValue = it->second.at(key).get<T>();
		return true;
	}

	// ファイルを読み込んで dataMap[path] に保存
	static bool Load(const std::string& path);
	// そのディレクトリの全てのファイルを読み込んで dataMap に保存
	static void LoadAll(const std::string& directoryPath);
	// dataMap[path] の内容をファイルに保存
	static bool Save(const std::string& path);
	// dataMap の全ての内容を対応するファイルに保存
	static void SaveAll();

private:
	static std::unordered_map<std::string, nlohmann::json> dataMap;
};

inline void to_json(nlohmann::json& j, const Vector2& v)
{
	j = nlohmann::json{ {"x", v.x}, {"y", v.y} };
}
inline void from_json(const nlohmann::json& j, Vector2& v)
{
	j.at("x").get_to(v.x);
	j.at("y").get_to(v.y);
}

inline void to_json(nlohmann::json& j, const Vector2int& v)
{
	j = nlohmann::json{ {"x", v.x}, {"y", v.y} };
}
inline void from_json(const nlohmann::json& j, Vector2int& v)
{
	j.at("x").get_to(v.x);
	j.at("y").get_to(v.y);
}

inline void to_json(nlohmann::json& j, const Vector3& v)
{
	j = nlohmann::json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
}
inline void from_json(const nlohmann::json& j, Vector3& v)
{
	j.at("x").get_to(v.x);
	j.at("y").get_to(v.y);
	j.at("z").get_to(v.z);
}

inline void to_json(nlohmann::json& j, const Vector3int& v)
{
	j = nlohmann::json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
}
inline void from_json(const nlohmann::json& j, Vector3int& v)
{
	j.at("x").get_to(v.x);
	j.at("y").get_to(v.y);
	j.at("z").get_to(v.z);
}

inline void to_json(nlohmann::json& j, const Vector4& v)
{
	j = nlohmann::json{ {"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w} };
}
inline void from_json(const nlohmann::json& j, Vector4& v)
{
	j.at("x").get_to(v.x);
	j.at("y").get_to(v.y);
	j.at("z").get_to(v.z);
	j.at("w").get_to(v.w);
}

inline void to_json(nlohmann::json& j, const AABB& aabb)
{
	j = nlohmann::json{ {"min", aabb.min}, {"max", aabb.max} };
}
inline void from_json(const nlohmann::json& j, AABB& aabb)
{
	j.at("min").get_to(aabb.min);
	j.at("max").get_to(aabb.max);
}

inline void to_json(nlohmann::json& j, const Matrix3x3& m)
{
	j = nlohmann::json{
	{ "m", nlohmann::json::array() }
	};

	for (int i = 0; i < 3; ++i)
	{
		j["m"].push_back({ m.m[i][0], m.m[i][1], m.m[i][2] });
	}

}
inline void from_json(const nlohmann::json& j, Matrix3x3& m)
{
	for (int i = 0; i < 3; ++i)
	{
		for (int k = 0; k < 3; ++k)
		{
			j.at("m").at(i).at(k).get_to(m.m[i][k]);
		}
	}
}

inline void to_json(nlohmann::json& j, const Matrix4x4& m)
{
	j = nlohmann::json{
	{ "m", nlohmann::json::array() }
	};

	for (int i = 0; i < 4; ++i)
	{
		j["m"].push_back({ m.m[i][0], m.m[i][1], m.m[i][2], m.m[i][3] });
	}
}
inline void from_json(const nlohmann::json& j, Matrix4x4& m)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int k = 0; k < 4; ++k)
		{
			j.at("m").at(i).at(k).get_to(m.m[i][k]);
		}
	}
}

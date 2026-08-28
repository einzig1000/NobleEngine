#include "ModelHelper.h"
#include <filesystem>
#include <fstream>

namespace ModelHelper
{
	ColliderShape LoadColliderShapes(const std::string& filePath, const std::vector<VertexData>& vertices)
	{
        auto path = std::filesystem::path(filePath);
        // ディレクトリ名
        std::string directory = path.parent_path().string();
        // 拡張子を除いたファイル名
        std::string stem = path.stem().string();
        // ColliderShape.csvのパス
        std::string csvFilePath = directory + "/" + stem + ".csv";

        ColliderShape colliderShapes;
        if (std::filesystem::exists(csvFilePath))
        {
            colliderShapes = LoadColliderShapesFromCSV(csvFilePath);
        }
        else
        {
            // 全頂点を含む形のデフォルトAABBを1つ作成
            AABB aabb = CreateDefaultAABB(vertices);
            colliderShapes.aabbs.push_back(aabb);
            SaveColliderShapesToCSV(csvFilePath, colliderShapes);
        }
        return colliderShapes;
	}

	ColliderShape LoadColliderShapesFromCSV(const std::string& filePath)
	{
        ColliderShape colliderShapes;
        std::ifstream file(filePath);
        if (!file.is_open()) return colliderShapes;

        std::string line;
        enum class Mode { None, Sphere, AABB };
        Mode mode = Mode::None;

        while (std::getline(file, line))
        {
            if (line.empty()) continue;

            // 構造読みこみ
            if (line == "center_x,center_y,center_z,radius")
            {
                mode = Mode::Sphere;
                continue;
            }
            if (line == "min_x,min_y,min_z,max_x,max_y,max_z")
            {
                mode = Mode::AABB;
                continue;
            }

            // 実データ読み込み
            std::istringstream ss(line);
            std::string token;
            std::vector<float> values;

            while (std::getline(ss, token, ','))
            {
                values.push_back(std::stof(token));
            }

            if (mode == Mode::Sphere && values.size() == 4)
            {
                Sphere sphere;
                sphere.center = { values[0], values[1], values[2] };
                sphere.radius = values[3];
                colliderShapes.spheres.push_back(sphere);
            }
            else if (mode == Mode::AABB && values.size() == 6)
            {
                AABB aabb;
                aabb.min = { values[0], values[1], values[2] };
                aabb.max = { values[3], values[4], values[5] };
                colliderShapes.aabbs.push_back(aabb);
            }
        }

        return colliderShapes;
	}

	AABB CreateDefaultAABB(const std::vector<VertexData>& vertices)
	{
        AABB aabb;

        // 最小値と最大値を初期化
        aabb.min.x = std::numeric_limits<float>::max();
        aabb.min.y = std::numeric_limits<float>::max();
        aabb.min.z = std::numeric_limits<float>::max();

        aabb.max.x = std::numeric_limits<float>::lowest();
        aabb.max.y = std::numeric_limits<float>::lowest();
        aabb.max.z = std::numeric_limits<float>::lowest();

        // 頂点データが空
        if (vertices.empty())
        {
            aabb.min = { 0.0f, 0.0f, 0.0f };
            aabb.max = { 0.0f, 0.0f, 0.0f };
            return aabb;
        }

        // 全ての頂点を調べてAABBの最小値と最大値を更新
        for (const auto& vertex : vertices)
        {
            // 各軸の最小値を更新
            if (vertex.position.x < aabb.min.x) aabb.min.x = vertex.position.x;
            if (vertex.position.y < aabb.min.y) aabb.min.y = vertex.position.y;
            if (vertex.position.z < aabb.min.z) aabb.min.z = vertex.position.z;

            // 各軸の最大値を更新
            if (vertex.position.x > aabb.max.x) aabb.max.x = vertex.position.x;
            if (vertex.position.y > aabb.max.y) aabb.max.y = vertex.position.y;
            if (vertex.position.z > aabb.max.z) aabb.max.z = vertex.position.z;
        }

        return aabb;
	}

	void SaveColliderShapesToCSV(const std::string& filePath, const ColliderShape& colliderShapes)
	{
		std::ofstream file(filePath);

		file << "center_x,center_y,center_z,radius\n";
		for (const auto& sphere : colliderShapes.spheres)
		{
			file << sphere.center.x << "," << sphere.center.y << "," << sphere.center.z << "," << sphere.radius << "\n";
		}

		file << "min_x,min_y,min_z,max_x,max_y,max_z\n";
		for (const auto& aabb : colliderShapes.aabbs)
		{
			file << aabb.min.x << "," << aabb.min.y << "," << aabb.min.z << ","
				<< aabb.max.x << "," << aabb.max.y << "," << aabb.max.z << "\n";
		}
	}
}
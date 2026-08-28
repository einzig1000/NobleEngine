#pragma once
#include <EngineDefinition/EngineDefinition.h>

namespace ModelHelper
{
	// ColliderShape読み込み
	ColliderShape LoadColliderShapes(const std::string& filePath, const std::vector<VertexData>& vertices);
	
	// ColliderShape.csvがあれば読み込み
	ColliderShape LoadColliderShapesFromCSV(const std::string& filePath);
	
	// 指定頂点すべてを含むAABBを作成
	AABB CreateDefaultAABB(const std::vector<VertexData>& vertices);
	
	// ColliderShapeをCSVに保存
	void SaveColliderShapesToCSV(const std::string& filePath, const ColliderShape& colliderShapes);

};


#pragma once
#include <definition/definition.h>
#include <cstdint>
#include <vector>
#include <assimp/mesh.h>

struct ResMeshlet
{
	uint32_t vertexOffset = 0;		// 頂点番号オフセット
	uint32_t vertexCount = 0;		// 頂点数
	uint32_t primitiveOffset = 0;	// プリミティブ番号オフセット
	uint32_t primitiveCount = 0;	// プリミティブ数
};

//struct ResPrimitiveIndex
//{
//	uint32_t index0 : 10;	// 出力頂点番号 (10bit)
//	uint32_t index1 : 10;	// 出力頂点番号 (10bit)
//	uint32_t index2 : 10;	// 出力頂点番号 (10bit)
//	uint32_t reserved : 2;	// 予約領域 (2bit)
//};

struct ResPrimitiveIndex
{
	uint32_t index = 0;	// 10bit * 3 = 30bit, 残り2bitは予約領域
};

struct ResMesh
{
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	uint32_t materialID = 0;

	std::vector<ResMeshlet> meshlets;
	std::vector<uint32_t> uniqueVertexIndices;
	std::vector<ResPrimitiveIndex> primitiveIndices;
};


class MeshLoader
{
public:
	void ParseMesh(ResMesh& mesh, const aiMesh* aiMeshPtr);
};

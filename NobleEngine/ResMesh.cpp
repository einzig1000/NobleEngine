#include "ResMesh.h"
#include <externals/meshoptimizer-1.1/meshoptimizer.h>

//重複頂点の削除(Remap)
//　meshopt_generateVertexRemap
//　meshopt_remapVertexBuffer
//　meshopt_remapIndexBuffer
//
//頂点キャッシュの最適化
//　meshopt_optimizeVertexCache
//
//頂点フェッチの最適化
//　meshopt_optimizeVertexFetch
//
//メッシュレットの構築
//　meshopt_buildMeshlets
//　※最大頂点数(max_vertices) = 64、最大プリミティブ数(max_triangles) = 126 に設定。

void MeshLoader::ParseMesh(ResMesh& mesh, const aiMesh* aiMeshPtr)
{
	// マテリアルIDを設定
	mesh.materialID = aiMeshPtr->mMaterialIndex;

	aiVector3D zero3D(0.0f, 0.0f, 0.0f);

	// 頂点データのメモリを確保
	mesh.vertices.resize(aiMeshPtr->mNumVertices);

	for (uint32_t vertexIndex = 0; vertexIndex < aiMeshPtr->mNumVertices; ++vertexIndex)
	{
		aiVector3D& position = aiMeshPtr->mVertices[vertexIndex];
		aiVector3D& normal = aiMeshPtr->HasNormals() ? aiMeshPtr->mNormals[vertexIndex] : zero3D;
		aiVector3D& texCoord = aiMeshPtr->HasTextureCoords(0) ? aiMeshPtr->mTextureCoords[0][vertexIndex] : zero3D;
		mesh.vertices[vertexIndex].position = { position.x, position.y, position.z };
		mesh.vertices[vertexIndex].normal = { normal.x, normal.y, normal.z };
		mesh.vertices[vertexIndex].texcoord = { texCoord.x, texCoord.y };
	}

	// 頂点インデックスのメモリを確保
	mesh.indices.resize(aiMeshPtr->mNumFaces * 3);

	for (uint32_t faceIndex = 0; faceIndex < aiMeshPtr->mNumFaces; ++faceIndex)
	{
		const aiFace& face = aiMeshPtr->mFaces[faceIndex];
		assert(face.mNumIndices == 3);
		mesh.indices[faceIndex * 3 + 0] = face.mIndices[0];
		mesh.indices[faceIndex * 3 + 1] = face.mIndices[1];
		mesh.indices[faceIndex * 3 + 2] = face.mIndices[2];
	}

	// 最適化
	{
		std::vector<uint32_t> remap(mesh.vertices.size());

		size_t vertexCount = meshopt_generateVertexRemap(
			remap.data(),
			mesh.indices.data(),
			mesh.indices.size(), 
			mesh.vertices.data(), 
			mesh.vertices.size(),
			sizeof(VertexData));

		std::vector<VertexData> vertices(vertexCount);
		std::vector<uint32_t> indices(mesh.indices.size());

		// インデックスバッファをリマップ
		meshopt_remapIndexBuffer(
			indices.data(),
			mesh.indices.data(),
			mesh.indices.size(),
			remap.data());

		// 頂点データをリマップ
		meshopt_remapVertexBuffer(
			vertices.data(),
			mesh.vertices.data(),
			mesh.vertices.size(),
			sizeof(VertexData),
			remap.data());

		// 最適なサイズに圧縮
		mesh.vertices.resize(vertices.size());
		mesh.indices.resize(indices.size());

		// 頂点キャッシュ最適化
		meshopt_optimizeVertexCache(
			mesh.indices.data(),
			indices.data(),
			indices.size(),
			vertexCount); 

		// オーバードロー最適化
		meshopt_optimizeOverdraw(
				mesh.indices.data(),
				mesh.indices.data(),
				mesh.indices.size(),
				&vertices[0].position.x,
				vertices.size(),
				sizeof(VertexData),
				1.05f);

		// 頂点フェッチ最適化
		meshopt_optimizeVertexFetch(
			mesh.vertices.data(),
			mesh.indices.data(),
			mesh.indices.size(),
			vertices.data(),
			vertices.size(),
			sizeof(VertexData));
	}

	// メッシュレットの生成
	{
		const size_t kMaxVertices = 64;
		const size_t kMaxPrimitives = 126;

		const size_t maxMeshlets = meshopt_buildMeshletsBound(mesh.indices.size(), kMaxVertices, kMaxPrimitives);

		std::vector<meshopt_Meshlet> meshlets(maxMeshlets);
		std::vector<unsigned int> meshletVertices(maxMeshlets * kMaxVertices);
		std::vector<unsigned char> meshletTriangles(maxMeshlets * kMaxPrimitives * 3);

		//template <typename T>
		//inline size_t meshopt_buildMeshlets(
		// meshopt_Meshlet * meshlets,
		// unsigned int* meshlet_vertices, 
		// unsigned char* meshlet_triangles, 
		// const T * indices,
		// size_t index_count, 
		// const float* vertex_positions, 
		// size_t vertex_count, 
		// size_t vertex_positions_stride,
		// size_t max_vertices, 
		// size_t max_triangles, 
		// float cone_weight)
		
		//size_t meshlet_count = meshopt_buildMeshlets(
		//	meshlets.data(),          // [出] メッシュレットのメタ情報（オフセットなど）が格納される配列
		//	meshlet_vertices.data(),  // [出] 元の頂点バッファへのインデックス（ローカル頂点マップ）
		//	meshlet_triangles.data(), // [出] メッシュレット内のローカルな三角形インデックス（3の倍数）
		//	indices.data(),           // [入] 元のメッシュのインデックスバッファ
		//	indices.size(),           // [入] 元のメッシュのインデックス数
		//	&vertices[0].x,           // [入] 頂点座標（Vector3等）の先頭ポインタ（クラスタリングの計算に使用）
		//	vertices.size(),          // [入] 元のメッシュの頂点数
		//	sizeof(Vertex),           // [入] 頂点構造体のストライド（バイトサイズ）
		//	max_vertices,             // メッシュレットごとの最大頂点数限制
		//	max_triangles,            // メッシュレットごとの最大三角形数限制
		//	cone_weight               // コーンカリングの重み（0.0 ～ 1.0）
		//);

		size_t meshletCount =
			meshopt_buildMeshlets(
				meshlets.data(),
				meshletVertices.data(),
				meshletTriangles.data(),
				mesh.indices.data(),
				mesh.indices.size(),
				&mesh.vertices[0].position.x,
				mesh.vertices.size(),
				sizeof(VertexData),
				kMaxVertices,
				kMaxPrimitives,
				0.25f
			);



		if (meshletCount > 0)
		{
			// 不要メモリを解放
			const meshopt_Meshlet& last = meshlets[meshletCount - 1];
			meshletVertices.resize(last.vertex_offset + last.vertex_count);
			meshletTriangles.resize(last.triangle_offset + last.triangle_count * 3);
			meshlets.resize(meshletCount);

			// 各メッシュレットの内部を最適化
			for (size_t i = 0; i < meshletCount; ++i)
			{
				meshopt_Meshlet& m = meshlets[i];
				meshopt_optimizeMeshlet(
					&meshletVertices[m.vertex_offset],
					&meshletTriangles[m.triangle_offset],
					m.triangle_count,
					m.vertex_count
				);
			}
		}

		mesh.uniqueVertexIndices.reserve(meshletCount* kMaxVertices);
		mesh.primitiveIndices.reserve(meshletCount* kMaxPrimitives * 3);

		for (auto& meshlet : meshlets)
		{
			uint32_t vertexOffset = uint32_t(mesh.uniqueVertexIndices.size());
			uint32_t primitiveOffset = uint32_t(mesh.primitiveIndices.size());

			for (unsigned int i = 0; i < meshlet.vertex_count; ++i)
			{
				mesh.uniqueVertexIndices.push_back(meshletVertices[meshlet.vertex_offset + i]);
			}

			for (unsigned int i = 0; i < meshlet.triangle_count * 3; ++i)
			{
				ResPrimitiveIndex tris{};
				uint32_t baseIndex = meshlet.triangle_offset + (i * 3);
				tris.index0 = meshletTriangles[baseIndex];
				tris.index1 = meshletTriangles[baseIndex + 1];
				tris.index2 = meshletTriangles[baseIndex + 2];
				mesh.primitiveIndices.push_back(tris);
			}

			// メッシュレットデータ設定
			ResMeshlet resMeshlet{};
			resMeshlet.vertexCount = meshlet.vertex_count;
			resMeshlet.vertexOffset = vertexOffset;
			resMeshlet.primitiveCount = meshlet.triangle_count;
			resMeshlet.primitiveOffset = primitiveOffset;

			mesh.meshlets.push_back(resMeshlet);
		}

		// サイズ最適化
		mesh.uniqueVertexIndices.shrink_to_fit();
		mesh.primitiveIndices.shrink_to_fit();
		mesh.meshlets.shrink_to_fit();
	}
}

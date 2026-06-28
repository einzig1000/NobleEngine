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

			for (unsigned int i = 0; i < meshlet.triangle_count; ++i)
			{
				ResPrimitiveIndex tris{};
				uint32_t baseIndex = meshlet.triangle_offset + (i * 3);
				tris.index = (meshletTriangles[baseIndex]) |
					(meshletTriangles[baseIndex + 1] << 10) |
					(meshletTriangles[baseIndex + 2] << 20);
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


template <typename T>
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> UploadBufferData(
	ID3D12Resource* buffer,
	const std::vector<T>& data,
	ID3D12Device2* device,
	ID3D12GraphicsCommandList6* commandList)
{
	D3D12_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pData = data.data();                         // データの先頭ポインタ
	subresourceData.RowPitch = data.size() * sizeof(T);          // 全体のバイトサイズ
	subresourceData.SlicePitch = subresourceData.RowPitch;        // バッファなのでRowPitchと同じ

	// 2. 中間リソース(Uploadヒープ)に必要なサイズを取得
	uint64_t intermediateSize = GetRequiredIntermediateSize(buffer, 0, 1);

	// 3. 既存のFactoryを利用して中間リソースを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource =
		Dx12ResourceFactory::CreateBufferResource(device, intermediateSize);

	// 4. コマンドリストにコピー処理を記録
	UpdateSubresources(commandList, buffer, intermediateResource.Get(), 0, 0, 1, &subresourceData);

	// 5. コピー先(COPY_DEST)から、シェーダー読み込み用(GENERIC_READ)へリソースバリアを張る
	// ※メッシュシェーダーおよびピクセルシェーダーのSRVとして安全に読むため、GENERIC_READが最適です
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = buffer;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;
}

// 使用例：特定の構造体データをGPUバッファ化してSRVを割り当てる関数（イメージ）
template <typename T>
int32_t CreateStructuredBufferSRV(const std::vector<T>& data)
{
	auto* device = dxManager_->GetDevice();
	auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);
	auto* srvManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();

	// 全体のバイトサイズを計算
	uint64_t bufferSize = data.size() * sizeof(T);

	// ① GPU側のデフォルトリソースを作成 (※FactoryにBuffer用があると仮定)
	// もし既存のFactoryがフラグ等を弄れない場合は、内部で以下のようにD3D12_RESOURCE_STATE_COPY_DESTで作成します。
	/*
	auto desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&gpuResource));
	*/
	Microsoft::WRL::ComPtr<ID3D12Resource> gpuResource = Dx12ResourceFactory::CreateBufferResource(device, bufferSize);

	// ② データのアップロード（上記で作った関数を呼び出し、中間リソースを保持）
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediate = UploadBufferData(gpuResource.Get(), data, device, cmdList);
	intermediateUploadResources_.push_back(intermediate); // 既存システム同様、フレーム終了まで維持

	// ③ StructuredBuffer用のSRVを作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;               // StructuredBufferの時は必ずUNKNOWNにする
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER; // バッファを指定
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(data.size());       // 要素数（配列の長さ）
	srvDesc.Buffer.StructureByteStride = static_cast<UINT>(sizeof(T)); // 1要素のバイトサイズ
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	// 既存のマネージャーからディスクリプタを割り当てて作成
	SRV_UAVManager::Allocation srvAllocation = srvManager->Allocate(); // ※既存の仕組みに合わせて調整してください
	device->CreateShaderResourceView(gpuResource.Get(), &srvDesc, srvAllocation.cpuHandle);

	// ④ バンク等に保存してインデックスを返す
	// bank_->AddBufferData(..., srvAllocation.index, ...);

	return srvAllocation.index;
}
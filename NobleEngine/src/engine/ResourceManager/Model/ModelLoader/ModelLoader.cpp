#include "ModelLoader.h"
#include <Utilities/Logger/Logger.h>
#include <DirectX/Resource/Dx12ResourceFactory.h>
#include <DirectX/DirectXManager.h>
#include <ResourceManager/Model/ModelBank/ModelBank.h>
#include <filesystem>
#include <fstream>
#include <externals/meshoptimizer-1.1/meshoptimizer.h>

namespace
{
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
        subresourceData.SlicePitch = subresourceData.RowPitch;       // バッファなのでRowPitchと同じ
        // 中間リソース(Uploadヒープ)に必要なサイズを取得
        uint64_t intermediateSize = GetRequiredIntermediateSize(buffer, 0, 1);
        // 既存のFactoryを利用して中間リソースを作成
        Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource =
            Dx12ResourceFactory::CreateBufferResource(device, intermediateSize);
        // コマンドリストにコピー処理を記録
        UpdateSubresources(commandList, buffer, intermediateResource.Get(), 0, 0, 1, &subresourceData);
        // ResourceStateをCOPY_DESTからGENERIC_READに変更
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
}

ModelLoader::ModelLoader(DirectXManager* dxManager, ModelBank* bank)
    : dxManager_(dxManager), bank_(bank)
{}

ModelLoader::~ModelLoader()
{}

int32_t ModelLoader::LoadModel(const std::string & filePath)
{
	// すでに読み込まれていたらそのモデルIDを返す
	int32_t existingModelID = bank_->IsModelDataExist(filePath);
	if (existingModelID != -1) return existingModelID;

    Log("モデル読み込み開始:%s", filePath.c_str());

    std::unique_ptr<ModelData> obj = std::make_unique<ModelData>();

    // モデルデータ読みこみ
    LoadModelFile(filePath, obj.get());

	// モデルバンクに登録
	int32_t modelID = bank_->AllocateModelID();
    bank_->AddModelData(filePath, modelID, std::move(obj));

    Log("成功 ID:%d", modelID);

    return modelID;
}

MaterialData ModelLoader::LoadMaterialTemplateFile(const std::string& filePath)
{
    return MaterialData();
}

void ModelLoader::LoadModelFile(const std::string& filePath, ModelData* modelData)
{
    uint32_t backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
    auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);
    auto* srvManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();


    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath.c_str(),
        aiProcess_Triangulate               // 面を三角形に分割する
        | aiProcess_ConvertToLeftHanded     // 左手座標系に変換する(逆に言うとこのエンジンで使用するモデルは右手座標系で作成する必要がある)
        | aiProcess_GenSmoothNormals        // 法線データが存在しないときに自動生成する
        | aiProcess_JoinIdenticalVertices   // 重複頂点を結合する
    );
    assert(scene->HasMeshes());

    // メッシュ取得
    aiMesh* mesh = scene->mMeshes[0];
	modelData->materialID = mesh->mMaterialIndex;

    // 各情報が存在するか
    const bool hasNormals = mesh->HasNormals();
    Log("法線データが存在しません。自動生成された数値が使用されます");
    const bool hasTexCoords = mesh->HasTextureCoords(0);
    Log("テクスチャ座標データが存在しません。(0.0f,0.0f)で初期化されます");

    modelData->vertices.resize(mesh->mNumVertices);
    for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
    {
        aiVector3D position = mesh->mVertices[vertexIndex];
		aiVector3D normal = hasNormals ? mesh->mNormals[vertexIndex] : aiVector3D(0.0f, 1.0f, 0.0f);
        aiVector3D texCoord = hasTexCoords ? mesh->mTextureCoords[0][vertexIndex] : aiVector3D(0.0f, 0.0f, 0.0f);

        modelData->vertices[vertexIndex].position = { position.x, position.y, position.z, 1.0f };
		modelData->vertices[vertexIndex].normal = { normal.x, normal.y, normal.z };
		modelData->vertices[vertexIndex].texcoord = { texCoord.x, texCoord.y };
    }

    modelData->indices.resize(mesh->mNumFaces * 3);
    for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
    {
        const aiFace& face = mesh->mFaces[faceIndex];
        assert(face.mNumIndices == 3); // 三角形でなければエラー

        modelData->indices[faceIndex * 3 + 0] = face.mIndices[0];
        modelData->indices[faceIndex * 3 + 1] = face.mIndices[1];
        modelData->indices[faceIndex * 3 + 2] = face.mIndices[2];
    }

	// 頂点データ & インデックスデータの最適化
    {
        std::vector<uint32_t> remap(modelData->vertices.size());

        size_t vertexCount = meshopt_generateVertexRemap(
            remap.data(),
            modelData->indices.data(),
            modelData->indices.size(),
            modelData->vertices.data(),
            modelData->vertices.size(),
            sizeof(VertexData));

        std::vector<VertexData> vertices(vertexCount);
        std::vector<uint32_t> indices(modelData->indices.size());

        // インデックスバッファをリマップ
        meshopt_remapIndexBuffer(
            indices.data(),
            modelData->indices.data(),
            modelData->indices.size(),
            remap.data());

        // 頂点データをリマップ
        meshopt_remapVertexBuffer(
            vertices.data(),
            modelData->vertices.data(),
            modelData->vertices.size(),
            sizeof(VertexData),
            remap.data());

        // 最適なサイズに圧縮
        modelData->vertices.resize(vertices.size());
        modelData->indices.resize(indices.size());

        // 頂点キャッシュ最適化
        meshopt_optimizeVertexCache(
            modelData->indices.data(),
            indices.data(),
            indices.size(),
            vertexCount);

        // オーバードロー最適化
        meshopt_optimizeOverdraw(
            modelData->indices.data(),
            modelData->indices.data(),
            modelData->indices.size(),
            &vertices[0].position.x,
            vertices.size(),
            sizeof(VertexData),
            1.05f);

        // 頂点フェッチ最適化
        meshopt_optimizeVertexFetch(
            modelData->vertices.data(),
            modelData->indices.data(),
            modelData->indices.size(),
            vertices.data(),
            vertices.size(),
            sizeof(VertexData));
    }

    // メッシュレットの生成
    {
        const size_t kMaxVertices = 64;
        const size_t kMaxPrimitives = 126;

        const size_t maxMeshlets = meshopt_buildMeshletsBound(modelData->indices.size(), kMaxVertices, kMaxPrimitives);

        std::vector<meshopt_Meshlet> meshlets(maxMeshlets);
        std::vector<unsigned int> meshletVertices(maxMeshlets * kMaxVertices);
        std::vector<unsigned char> meshletTriangles(maxMeshlets * kMaxPrimitives * 3);

        //template <typename T>
        //inline size_t meshopt_buildMeshlets(
        // meshopt_Meshlet * meshlets,          // [出] メッシュレットのメタ情報（オフセットなど）が格納される配列
        // unsigned int* meshlet_vertices,      // [出] 元の頂点バッファへのインデックス（ローカル頂点マップ）
        // unsigned char* meshlet_triangles,    // [出] メッシュレット内のローカルな三角形インデックス（3の倍数）
        // const T * indices,                   // [入] 元のメッシュのインデックスバッファ
        // size_t index_count,                  // [入] 元のメッシュのインデックス数
        // const float* vertex_positions,       // [入] 頂点座標（Vector3等）の先頭ポインタ（クラスタリングの計算に使用）
        // size_t vertex_count,                 // [入] 元のメッシュの頂点数
        // size_t vertex_positions_stride,      // [入] 頂点構造体のストライド（バイトサイズ）
        // size_t max_vertices,                 // メッシュレットごとの最大頂点数限制
        // size_t max_triangles,                // メッシュレットごとの最大三角形数限制
        // float cone_weight)                   // コーンカリングの重み（0.0 ～ 1.0）

        size_t meshletCount =
            meshopt_buildMeshlets(
                meshlets.data(),
                meshletVertices.data(),
                meshletTriangles.data(),
                modelData->indices.data(),
                modelData->indices.size(),
                &modelData->vertices[0].position.x,
                modelData->vertices.size(),
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

        modelData->uniqueVertexIndices.reserve(meshletCount * kMaxVertices);
        modelData->primitiveIndices.reserve(meshletCount * kMaxPrimitives * 3);

        for (auto& meshlet : meshlets)
        {
            uint32_t vertexOffset = uint32_t(modelData->uniqueVertexIndices.size());
            uint32_t primitiveOffset = uint32_t(modelData->primitiveIndices.size());

            for (unsigned int i = 0; i < meshlet.vertex_count; ++i)
            {
                modelData->uniqueVertexIndices.push_back(meshletVertices[meshlet.vertex_offset + i]);
            }

            for (unsigned int i = 0; i < meshlet.triangle_count; ++i)
            {
                uint32_t tris{};
                uint32_t baseIndex = meshlet.triangle_offset + (i * 3);
                tris = (meshletTriangles[baseIndex]) |
                    (meshletTriangles[baseIndex + 1] << 10) |
                    (meshletTriangles[baseIndex + 2] << 20);
                modelData->primitiveIndices.push_back(tris);
            }

            // メッシュレットデータ設定
            ResMeshlet resMeshlet{};
            resMeshlet.vertexCount = meshlet.vertex_count;
            resMeshlet.vertexOffset = vertexOffset;
            resMeshlet.primitiveCount = meshlet.triangle_count;
            resMeshlet.primitiveOffset = primitiveOffset;

            modelData->meshlets.push_back(resMeshlet);
        }

        // サイズ最適化
        modelData->uniqueVertexIndices.shrink_to_fit();
        modelData->primitiveIndices.shrink_to_fit();
        modelData->meshlets.shrink_to_fit();
    }

    modelData->rootNode = ReadNode(scene->mRootNode);

    /// スケルトン作成
    modelData->skeleton = CreateSkeleton(modelData->rootNode);

    /// AABB読み込み
    modelData->aabb = LoadAABB(filePath, modelData->vertices);

    /// ファイルパスを保存
    modelData->filePath = filePath;

    /// 頂点バッファ作成
    size_t vertexBufferSize = sizeof(VertexData) * modelData->vertices.size();
    modelData->vertexBuffer = Dx12ResourceFactory::CreateBufferResource(dxManager_->GetDevice(), vertexBufferSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferUpload = Dx12ResourceFactory::CreateBufferResource(dxManager_->GetDevice(), vertexBufferSize);
        UploadBufferData(
            modelData->meshletBuffer.Get(),
            modelData->meshlets,
            dxManager_->GetDevice(),
            cmdList);
    intermediateUploadResources_.push_back(vertexBufferUpload);

    VertexData* vData = nullptr;
    modelData->vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vData));
    std::memcpy(vData, modelData->vertices.data(), vertexBufferSize);
    modelData->vertexBuffer->Unmap(0, nullptr);
    modelData->vertexBufferView.BufferLocation = modelData->vertexBuffer->GetGPUVirtualAddress();
    modelData->vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
    modelData->vertexBufferView.StrideInBytes = sizeof(VertexData);

    //modelData->verte

    /// インデックスバッファ作成
    size_t indexBufferSize = sizeof(uint32_t) * UINT(modelData->indices.size());
    modelData->indexBuffer = Dx12ResourceFactory::CreateBufferResource(dxManager_->GetDevice(), indexBufferSize);
    uint32_t* iData = nullptr;
    modelData->indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&iData));
    std::memcpy(iData, modelData->indices.data(), indexBufferSize);
    modelData->indexBuffer->Unmap(0, nullptr);
    modelData->indexBufferView.BufferLocation = modelData->indexBuffer->GetGPUVirtualAddress();
    modelData->indexBufferView.SizeInBytes = static_cast<UINT>(indexBufferSize);
    modelData->indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	/// メッシュレットバッファ作成
	size_t meshletBufferSize = sizeof(ResMeshlet) * modelData->meshlets.size();
	modelData->meshletBuffer = Dx12ResourceFactory::CreateBufferResource(dxManager_->GetDevice(), meshletBufferSize);
	Microsoft::WRL::ComPtr<ID3D12Resource> meshletBufferUpload = Dx12ResourceFactory::CreateBufferResource(dxManager_->GetDevice(), meshletBufferSize);
        UploadBufferData(
            modelData->meshletBuffer.Get(),
            modelData->meshlets,
            dxManager_->GetDevice(),
            cmdList);
    intermediateUploadResources_.push_back(meshletBufferUpload);

    SRV_UAVManager::Allocation srvAllocation =
        dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateSRVforStructuredBuffer(
            modelData->meshletBuffer.Get(),
            modelData->meshlets.size(),
            sizeof(ResMeshlet));
	modelData->meshletSrvIndex = srvAllocation.index;

	// modelData->skinClusterDataを作成する
    for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
		// Joint(Bone)から情報を取得
        aiBone* bone = mesh->mBones[boneIndex];
        std::string jointName = bone->mName.C_Str();
        JointWeightData& jointWeightData = modelData->skinClusterData[jointName];

		// Inverced Bind Pose Matrixを取得
        aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
        aiVector3D scale, translate;
        aiQuaternion rotate;
        bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
        Matrix4x4 bindPoseMatrix = Matrix4x4::MakeAffineMatrix(
            { scale.x, scale.y, scale.z }, { rotate.x, -rotate.y, -rotate.z, rotate.w }, { -translate.x, translate.y, translate.z });
        jointWeightData.inverseBindPoseMatrix = bindPoseMatrix.Inverse();

        for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
        {
            jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
        }
    }

    /// スキンクラスタ作成
    modelData->skinCluster = CreateSkinCluster(modelData->skeleton, *modelData);
}

Node ModelLoader::ReadNode(const aiNode* node)
{
    Node result;
    aiVector3D scale;
    aiQuaternion rotate;
    aiVector3D translate;
    node->mTransformation.Decompose(scale, rotate, translate);
    result.transform.scale = { scale.x, scale.y, scale.z };
    result.transform.rotate = { rotate.x, -rotate.y, -rotate.z, rotate.w };
    result.transform.translate = { -translate.x, translate.y, translate.z };
    result.localMatrix = Matrix4x4::MakeAffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);
    result.name = node->mName.C_Str();
    result.children.resize(node->mNumChildren);
    for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
    {
        result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
    }
    return result;
}

SkinCluster ModelLoader::CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData)
{
	SkinCluster skinCluster;

	// palette用のResourceを作成
	skinCluster.paletteResource = Dx12ResourceFactory::CreateBufferResource(dxManager_->GetDevice(), sizeof(WellForGPU) * skeleton.joints.size());
	WellForGPU* wData = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&wData));
	skinCluster.mappedPalette = { wData, skeleton.joints.size() };
	SRV_UAVManager::Allocation allocation = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateSRVforStructuredBuffer(skinCluster.paletteResource.Get(), static_cast<UINT>(skeleton.joints.size()), sizeof(WellForGPU));
    skinCluster.paletteSrvHandle.first = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->GetCPUHandleAt(allocation.index);
   	skinCluster.paletteSrvHandle.second = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->GetGPUHandleAt(allocation.index);

	// influence用のResourceを作成
	skinCluster.influenceResource = Dx12ResourceFactory::CreateBufferResource(dxManager_->GetDevice(), sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* iData = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&iData));
	std::memset(iData, 0, sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.mappedInfluences = { iData, modelData.vertices.size() };
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = static_cast<UINT>(sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());
	std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), Matrix4x4::MakeIdentity4x4 );

	// modelDataのskinClusterDataからJointWeightDataを取得し、各JointのInverseBindPoseMatrixを保存する
	for (const auto& [jointName, jointWeightData] : modelData.skinClusterData)
	{
		auto jointIt = skeleton.jointIndexByName.find(jointName);
		if (jointIt != skeleton.jointIndexByName.end())
		{
			size_t jointIndex = jointIt->second;
			skinCluster.inverseBindPoseMatrices[jointIndex] = jointWeightData.inverseBindPoseMatrix;
			// 各頂点の影響を設定
			for (const auto& vertexWeight : jointWeightData.vertexWeights)
			{
				VertexInfluence& influence = skinCluster.mappedInfluences[vertexWeight.vertexIndex];
				for (int i = 0; i < 4; ++i)
				{
					if (influence.weights[i] == 0.0f)
					{
						influence.weights[i] = vertexWeight.weight;
						influence.jointIndices[i] = static_cast<uint32_t>(jointIndex);
						break;
					}
				}
			}
		}
	}

	return skinCluster;
}

// スケルトンの作成
Skeleton ModelLoader::CreateSkeleton(const Node& node)
{
    Skeleton skeleton;
    skeleton.root = CreateJoint(node, std::nullopt, skeleton.joints);

    for (size_t i = 0; i < skeleton.joints.size(); ++i)
    {
        skeleton.jointIndexByName[skeleton.joints[i].name] = int32_t(i);
    }

    return skeleton;
}

int32_t ModelLoader::CreateJoint(const Node& node, const std::optional<int32_t>& parentIndex, std::vector<Joint>& joints)
{
    Joint joint;
    joint.name = node.name;
    joint.localMatrix = node.localMatrix;
    joint.skeletonSpaceMatrix = Matrix4x4::MakeIdentity4x4();
    joint.transform = node.transform;
    joint.index = int32_t(joints.size());
    joint.parentIndex = parentIndex;
    joints.push_back(joint);
    for (const auto& child : node.children)
    {
        int32_t childIndex = CreateJoint(child, joint.index, joints);
        joints[joint.index].childrenIndex.push_back(childIndex);
    }
    return joint.index;
}


// AABB.csvの読み込み & 存在しなければ作成,保存
std::vector<AABB> ModelLoader::LoadAABB(const std::string& filePath, const std::vector<VertexData>& vertices)
{
    auto path = std::filesystem::path(filePath);
    // ディレクトリ名
    std::string directory = path.parent_path().string();
    // 拡張子を除いたファイル名
    std::string stem = path.stem().string();
	// AABB.csvのパス
	std::string csvFilePath = directory + "/" + stem + ".csv";

    std::vector<AABB> aabbs;
    if (std::filesystem::exists(csvFilePath))
    {
        aabbs = LoadAABBFromCSV(csvFilePath);
    }
    else
    {
        // 今までの方法でAABBを1つ作成
        AABB aabb = CreateLocalAABB(vertices);
        aabbs.push_back(aabb);
        SaveAABBToCSV(csvFilePath, aabbs);
    }
    return aabbs;
}
// AABB.csvの読み込み
std::vector<AABB> ModelLoader::LoadAABBFromCSV(const std::string& filePath)
{
    std::vector<AABB> aabbs;
    std::ifstream file(filePath);
    if (!file.is_open()) return aabbs;

    std::string line;
    // 1行目はヘッダーなのでスキップ
    std::getline(file, line);

    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string token;
        std::vector<float> values;
        while (std::getline(ss, token, ','))
        {
            values.push_back(std::stof(token));
        }
        if (values.size() == 6)
        {
            AABB aabb;
            aabb.min = { values[0], values[1], values[2] };
            aabb.max = { values[3], values[4], values[5] };
            aabbs.push_back(aabb);
        }
    }
    return aabbs;
}
// AABBの作成
AABB ModelLoader::CreateLocalAABB(const std::vector<VertexData>& vertices)
{
    AABB localAABB;

    // 最小値と最大値を初期化
    localAABB.min.x = (std::numeric_limits<float>::max)();
    localAABB.min.y = (std::numeric_limits<float>::max)();
    localAABB.min.z = (std::numeric_limits<float>::max)();

    localAABB.max.x = std::numeric_limits<float>::lowest();

    // 頂点データ空だったらエラー出すべきだけどunityシステムあるかもだから落とさない
    if (vertices.empty())
    {
        localAABB.min = { 0.0f, 0.0f, 0.0f };
        localAABB.max = { 0.0f, 0.0f, 0.0f };
        return localAABB;
    }

    // 全ての頂点を調べてAABBの最小値と最大値を更新
    for (const auto& vertex : vertices)
    {
        // 各軸の最小値を更新
        if (vertex.position.x < localAABB.min.x) localAABB.min.x = vertex.position.x;
        if (vertex.position.y < localAABB.min.y) localAABB.min.y = vertex.position.y;
        if (vertex.position.z < localAABB.min.z) localAABB.min.z = vertex.position.z;

        // 各軸の最大値を更新
        if (vertex.position.x > localAABB.max.x) localAABB.max.x = vertex.position.x;
        if (vertex.position.y > localAABB.max.y) localAABB.max.y = vertex.position.y;
        if (vertex.position.z > localAABB.max.z) localAABB.max.z = vertex.position.z;
    }

    return localAABB;
}
// AABB.csvの作成、保存
void ModelLoader::SaveAABBToCSV(const std::string& filePath, const std::vector<AABB>& aabbs)
{
    std::ofstream file(filePath);
    file << "min_x,min_y,min_z,max_x,max_y,max_z\n";
    for (const auto& aabb : aabbs)
    {
        file << aabb.min.x << "," << aabb.min.y << "," << aabb.min.z << ","
            << aabb.max.x << "," << aabb.max.y << "," << aabb.max.z << "\n";
    }
}


//
//// 使用例：特定の構造体データをGPUバッファ化してSRVを割り当てる関数（イメージ）
//template <typename T>
//int32_t CreateStructuredBufferSRV(const std::vector<T>& data)
//{
//    auto* device = dxManager_->GetDevice();
//    auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
//    auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);
//    auto* srvManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();
//
//    // 全体のバイトサイズを計算
//    uint64_t bufferSize = data.size() * sizeof(T);
//
//    // ① GPU側のデフォルトリソースを作成 (※FactoryにBuffer用があると仮定)
//    // もし既存のFactoryがフラグ等を弄れない場合は、内部で以下のようにD3D12_RESOURCE_STATE_COPY_DESTで作成します。
//    /*
//    auto desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
//    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
//    device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
//        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&gpuResource));
//    */
//    Microsoft::WRL::ComPtr<ID3D12Resource> gpuResource = Dx12ResourceFactory::CreateBufferResource(device, bufferSize);
//
//    // ② データのアップロード（上記で作った関数を呼び出し、中間リソースを保持）
//    Microsoft::WRL::ComPtr<ID3D12Resource> intermediate = UploadBufferData(gpuResource.Get(), data, device, cmdList);
//    intermediateUploadResources_.push_back(intermediate); // 既存システム同様、フレーム終了まで維持
//
//    // ③ StructuredBuffer用のSRVを作成
//    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//    srvDesc.Format = DXGI_FORMAT_UNKNOWN;               // StructuredBufferの時は必ずUNKNOWNにする
//    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER; // バッファを指定
//    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//    srvDesc.Buffer.FirstElement = 0;
//    srvDesc.Buffer.NumElements = static_cast<UINT>(data.size());       // 要素数（配列の長さ）
//    srvDesc.Buffer.StructureByteStride = static_cast<UINT>(sizeof(T)); // 1要素のバイトサイズ
//    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
//
//    // 既存のマネージャーからディスクリプタを割り当てて作成
//    SRV_UAVManager::Allocation srvAllocation = srvManager->Allocate(); // ※既存の仕組みに合わせて調整してください
//    device->CreateShaderResourceView(gpuResource.Get(), &srvDesc, srvAllocation.cpuHandle);
//
//    // ④ バンク等に保存してインデックスを返す
//    // bank_->AddBufferData(..., srvAllocation.index, ...);
//
//    return srvAllocation.index;
//}
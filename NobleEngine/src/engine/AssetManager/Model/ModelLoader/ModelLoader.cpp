#include "ModelLoader.h"
#include <Utilities/Logger/Logger.h>
#include <DirectX/ResourceUtilities/ResourceUtilities.h>
#include <DirectX/DirectXManager.h>
#include <AssetManager/Model/ModelBank/ModelBank.h>
#include <AssetManager/Model/ModelHelper/ModelHelper.h>
#include <filesystem>
#include <fstream>
#include <externals/meshoptimizer-1.1/meshoptimizer.h>

namespace
{
    Matrix4x4 aiMatrix4x4ToMatrix4x4(const aiMatrix4x4& aiMat)
	{
		Matrix4x4 result;

        result.m[0][0] = aiMat.a1;
        result.m[1][0] = aiMat.a2;
        result.m[2][0] = aiMat.a3;
        result.m[3][0] = aiMat.a4;
        result.m[0][1] = aiMat.b1;
        result.m[1][1] = aiMat.b2;
        result.m[2][1] = aiMat.b3;
        result.m[3][1] = aiMat.b4;
        result.m[0][2] = aiMat.c1;
        result.m[1][2] = aiMat.c2;
        result.m[2][2] = aiMat.c3;
        result.m[3][2] = aiMat.c4;
        result.m[0][3] = aiMat.d1;
        result.m[1][3] = aiMat.d2;
        result.m[2][3] = aiMat.d3;
        result.m[3][3] = aiMat.d4;

		return result;
	}
}

ModelLoader::ModelLoader(DirectXManager* dxManager, ModelBank* bank)
    : dxManager_(dxManager), bank_(bank)
{}

ModelLoader::~ModelLoader()
{
	intermediateUploadResources_.clear();
}

int32_t ModelLoader::LoadModel(const std::string & filePath)
{
	// すでに読み込まれていたらそのモデルIDを返す
	int32_t modelID = bank_->IsModelDataExist(filePath);
	if (modelID != -1) return modelID;

    Log("モデル読み込み開始:%s", filePath.c_str());

    std::unique_ptr<ModelData> obj = std::make_unique<ModelData>();

    // モデルデータ読みこみ
    if (!LoadModelFile(filePath, obj.get()))
    {
        return -1;
    }

	// モデルバンクに登録
	modelID = bank_->AddModelData(filePath, std::move(obj));

    Log("成功 ID:%d", modelID);

    return modelID;
}

MaterialData ModelLoader::LoadMaterialTemplateFile(const std::string& filePath)
{
    return MaterialData();
}

bool ModelLoader::LoadModelFile(const std::string& filePath, ModelData* modelData)
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
    if (!scene)
    {
        Log("モデルファイルを開けませんでした:%s", filePath.c_str());
        Log("%s", importer.GetErrorString());
        assert(false);
		return false;
    }

	if (!scene->HasMeshes())
	{
		Log("モデルが含まれていません:%s", filePath.c_str());
		assert(false);
        return false;
	}

    // メッシュ取得
    aiMesh* mesh = scene->mMeshes[0];
	modelData->materialID = mesh->mMaterialIndex;

    // 各情報が存在するか
    const bool hasNormals = mesh->HasNormals();
    if (!hasNormals) Log("法線データが存在しません。自動生成された数値が使用されます");
    const bool hasTexCoords = mesh->HasTextureCoords(0);
    if (!hasTexCoords) Log("テクスチャ座標データが存在しません。(0.0f,0.0f)で初期化されます");

    // 頂点データ & インデックスデータの読み込み
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
    //{
    //    std::vector<uint32_t> remap(modelData->vertices.size());

    //    size_t vertexCount = meshopt_generateVertexRemap(
    //        remap.data(),
    //        modelData->indices.data(),
    //        modelData->indices.size(),
    //        modelData->vertices.data(),
    //        modelData->vertices.size(),
    //        sizeof(VertexData));

    //    std::vector<VertexData> vertices(vertexCount);
    //    std::vector<uint32_t> indices(modelData->indices.size());

    //    // インデックスバッファをリマップ
    //    meshopt_remapIndexBuffer(
    //        indices.data(),
    //        modelData->indices.data(),
    //        modelData->indices.size(),
    //        remap.data());

    //    // 頂点データをリマップ
    //    meshopt_remapVertexBuffer(
    //        vertices.data(),
    //        modelData->vertices.data(),
    //        modelData->vertices.size(),
    //        sizeof(VertexData),
    //        remap.data());

    //    // 最適なサイズに圧縮
    //    modelData->vertices.resize(vertices.size());
    //    modelData->indices.resize(indices.size());

    //    // 頂点キャッシュ最適化
    //    meshopt_optimizeVertexCache(
    //        modelData->indices.data(),
    //        indices.data(),
    //        indices.size(),
    //        vertexCount);

    //    // オーバードロー最適化
    //    meshopt_optimizeOverdraw(
    //        modelData->indices.data(),
    //        modelData->indices.data(),
    //        modelData->indices.size(),
    //        &vertices[0].position.x,
    //        vertices.size(),
    //        sizeof(VertexData),
    //        1.05f);

    //    // 頂点フェッチ最適化
    //    meshopt_optimizeVertexFetch(
    //        modelData->vertices.data(),
    //        modelData->indices.data(),
    //        modelData->indices.size(),
    //        vertices.data(),
    //        vertices.size(),
    //        sizeof(VertexData));
    //}

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
                    size_t(m.triangle_count),
                    size_t(m.vertex_count)
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
            resMeshlet.vertexCount = uint32_t(meshlet.vertex_count);
            resMeshlet.vertexOffset = vertexOffset;
            resMeshlet.primitiveCount = uint32_t(meshlet.triangle_count);
            resMeshlet.primitiveOffset = primitiveOffset;

            modelData->meshlets.push_back(resMeshlet);
        }

        // サイズ最適化
        modelData->uniqueVertexIndices.shrink_to_fit();
        modelData->primitiveIndices.shrink_to_fit();
        modelData->meshlets.shrink_to_fit();
    }

    Node rootNode = ReadNode(scene->mRootNode);

    /// スケルトン作成
    modelData->skeleton = CreateSkeleton(rootNode);
    
    /// ColliderShape読み込み
    modelData->colliderShape = ModelHelper::LoadColliderShapes(filePath, modelData->vertices);

    /// ファイルパスを保存
    modelData->filePath = filePath;

    /// 頂点バッファ作成
    {
        size_t bufferSize = sizeof(VertexData) * modelData->vertices.size();
        modelData->vertexBuffer = Dx12ResourceFactory::CreateDefaultBufferResource(dxManager_->GetDevice(), bufferSize);

        Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer = Dx12ResourceFactory::CreateUploadResource(modelData->vertexBuffer.Get(), modelData->vertices, dxManager_->GetDevice(), cmdList);
        intermediateUploadResources_.push_back(uploadBuffer);

        modelData->vertexBufferView.BufferLocation = modelData->vertexBuffer->GetGPUVirtualAddress();
        modelData->vertexBufferView.SizeInBytes = static_cast<UINT>(bufferSize);
        modelData->vertexBufferView.StrideInBytes = sizeof(VertexData);

        SRV_UAVManager::Allocation srvAllocation = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateSRVforStructuredBuffer(
            modelData->vertexBuffer.Get(),
            UINT(modelData->vertices.size()),
            sizeof(VertexData));
        modelData->vertexHeapSlot = srvAllocation.index;
    }

    /// インデックスバッファ作成
    {
        size_t bufferSize = sizeof(uint32_t) * UINT(modelData->indices.size());
        modelData->indexBuffer = Dx12ResourceFactory::CreateBufferResource(dxManager_->GetDevice(), bufferSize);
        uint32_t* iData = nullptr;
        modelData->indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&iData));
        std::memcpy(iData, modelData->indices.data(), bufferSize);
        modelData->indexBuffer->Unmap(0, nullptr);
        modelData->indexBufferView.BufferLocation = modelData->indexBuffer->GetGPUVirtualAddress();
        modelData->indexBufferView.SizeInBytes = static_cast<UINT>(bufferSize);
        modelData->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    }

	/// メッシュレットバッファ作成
    {
        size_t bufferSize = sizeof(ResMeshlet) * modelData->meshlets.size();
        modelData->meshletBuffer = Dx12ResourceFactory::CreateDefaultBufferResource(dxManager_->GetDevice(), bufferSize);
        
        Microsoft::WRL::ComPtr<ID3D12Resource> meshletBufferUpload = Dx12ResourceFactory::CreateUploadResource(modelData->meshletBuffer.Get(), modelData->meshlets, dxManager_->GetDevice(), cmdList);
        intermediateUploadResources_.push_back(meshletBufferUpload);

        SRV_UAVManager::Allocation srvAllocation = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateSRVforStructuredBuffer(
            modelData->meshletBuffer.Get(),
            UINT(modelData->meshlets.size()),
            sizeof(ResMeshlet));
        modelData->meshletHeapSlot = srvAllocation.index;
    }

	/// ユニーク頂点インデックスバッファ作成
    {
        size_t bufferSize = sizeof(uint32_t) * modelData->uniqueVertexIndices.size();
        modelData->uniqueVertexIndexBuffer = Dx12ResourceFactory::CreateDefaultBufferResource(dxManager_->GetDevice(), bufferSize);
        
        Microsoft::WRL::ComPtr<ID3D12Resource> uniqueVertexIndexBufferUpload = Dx12ResourceFactory::CreateUploadResource(modelData->uniqueVertexIndexBuffer.Get(), modelData->uniqueVertexIndices, dxManager_->GetDevice(), cmdList);
        intermediateUploadResources_.push_back(uniqueVertexIndexBufferUpload);

        SRV_UAVManager::Allocation srvAllocation = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateSRVforStructuredBuffer(
            modelData->uniqueVertexIndexBuffer.Get(),
            UINT(modelData->uniqueVertexIndices.size()),
            sizeof(uint32_t));
        modelData->uniqueVertexIndexHeapSlot = srvAllocation.index;
    }

	/// プリミティブインデックスバッファ作成
    {
        size_t bufferSize = sizeof(uint32_t) * modelData->primitiveIndices.size();
        modelData->primitiveIndexBuffer = Dx12ResourceFactory::CreateDefaultBufferResource(dxManager_->GetDevice(), bufferSize);
        
        Microsoft::WRL::ComPtr<ID3D12Resource> primitiveIndexBufferUpload = Dx12ResourceFactory::CreateUploadResource(modelData->primitiveIndexBuffer.Get(), modelData->primitiveIndices, dxManager_->GetDevice(), cmdList);
        intermediateUploadResources_.push_back(primitiveIndexBufferUpload);

        SRV_UAVManager::Allocation srvAllocation = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateSRVforStructuredBuffer(
            modelData->primitiveIndexBuffer.Get(),
            UINT(modelData->primitiveIndices.size()),
            sizeof(uint32_t));
        modelData->primitiveIndexHeapSlot = srvAllocation.index;
    }

    // 骨ごとのウェイトを集める
    std::map<std::string, JointWeightData> skinClusterData;
    for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        // Joint(Bone)から情報を取得
        aiBone* bone = mesh->mBones[boneIndex];
        std::string jointName = bone->mName.C_Str();
        JointWeightData& jointWeightData = skinClusterData[jointName];
        jointWeightData.inverseBindPoseMatrix = aiMatrix4x4ToMatrix4x4(bone->mOffsetMatrix);

        for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
        {
            jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
        }
    }

    /// スキンクラスタ作成
    modelData->skinBindData = CreateSkinBindData(modelData, skinClusterData);

	return true;
}

Node ModelLoader::ReadNode(const aiNode* node)
{
    Node result;
    aiVector3D scale;
    aiQuaternion rotate;
    aiVector3D translate;
    node->mTransformation.Decompose(scale, rotate, translate);
    result.transform.scale = { scale.x, scale.y, scale.z };
    result.transform.rotate = { rotate.x, rotate.y, rotate.z, rotate.w };
    result.transform.translate = { translate.x, translate.y, translate.z };
	result.localMatrix = aiMatrix4x4ToMatrix4x4(node->mTransformation);

    result.name = node->mName.C_Str();
    result.children.resize(node->mNumChildren);
    for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
    {
        result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
    }
    return result;
}

SkinBindData ModelLoader::CreateSkinBindData(const ModelData* modelData, const std::map<std::string, JointWeightData>& skinClusterData)
{
    auto backbufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
    auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backbufferIndex);

    SkinBindData bind;

    if (skinClusterData.empty() || modelData->vertices.empty())
    {
        return bind;
    }

    const size_t jointCount = modelData->skeleton.joints.size();
    const size_t vertexCount = modelData->vertices.size();

    // バインドポーズ逆行列
    bind.inverseBindPoseMatrices.assign(jointCount, Matrix4x4::MakeIdentity4x4());

    // influence
    std::vector<VertexInfluence> influences(vertexCount);
    for (const auto& [jointName, jointWeightData] : skinClusterData)
    {
        auto jointIt = modelData->skeleton.jointIndexByName.find(jointName);
        if (jointIt == modelData->skeleton.jointIndexByName.end()) continue;

        const size_t jointIndex = jointIt->second;
        assert(jointIndex < jointCount);
        bind.inverseBindPoseMatrices[jointIndex] = jointWeightData.inverseBindPoseMatrix;

        // 各頂点の影響を設定
        for (const auto& vertexWeight : jointWeightData.vertexWeights)
        {
            assert(vertexWeight.vertexIndex < vertexCount);
            VertexInfluence& influence = influences[vertexWeight.vertexIndex];
            for (size_t i = 0; i < 4; ++i)
            {
                if (influence.weights[i] == 0.0f)
                {
                    influence.weights[i] = vertexWeight.weight;
                    influence.jointIndices[i] = static_cast<int32_t>(jointIndex);
                    break;
                }
            }
        }
    }

    // DEFAULTヒープへアップ(頂点バッファと同じ流儀)
    bind.influenceBuffer = Dx12ResourceFactory::CreateDefaultBufferResource(dxManager_->GetDevice(), sizeof(VertexInfluence) * vertexCount);

    intermediateUploadResources_.push_back(
        Dx12ResourceFactory::CreateUploadResource(bind.influenceBuffer.Get(), influences, dxManager_->GetDevice(), cmdList));

    // ---- StructuredBufferとしてのSRVを作り、ヒープインデックスを保持する ----
    // ここでインデックスを持っておくことで、アプリ側の CreateStatic() による二重確保が不要になる
    bind.influenceHeapSlot = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->CreateSRVforStructuredBuffer(
        bind.influenceBuffer.Get(),
        static_cast<UINT>(vertexCount),
        static_cast<UINT>(sizeof(VertexInfluence))).index;

    return bind;
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

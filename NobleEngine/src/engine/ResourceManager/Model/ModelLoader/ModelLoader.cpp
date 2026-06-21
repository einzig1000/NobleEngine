#include "ModelLoader.h"
#include <Utilities/Logger/Logger.h>
#include <DirectX/Resource/Dx12ResourceFactory.h>
#include <ResourceManager/Model/ModelBank/ModelBank.h>
#include <filesystem>
#include <fstream>

ModelLoader::ModelLoader(ID3D12Device2* device, ModelBank* bank)
    : device_(device), bank_(bank)
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

	// 頂点配列・ノードを読み込む
    LoadModelFile(filePath, obj.get());

    // スケルトン作成
	obj->skeleton = CreateSkeleton(obj->rootNode);
    
    // AABB読み込み
    obj->aabb = LoadAABB(filePath, obj->vertices);

	// ファイルパスを保存
	obj->filePath = filePath;

    // 頂点バッファ作成
    size_t vertexBufferSize = sizeof(VertexData) * obj->vertices.size();
    obj->vertexBuffer = Dx12ResourceFactory::CreateBufferResource(device_, vertexBufferSize);
    VertexData* vData = nullptr;
    obj->vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vData));
    std::memcpy(vData, obj->vertices.data(), vertexBufferSize);
    obj->vertexBuffer->Unmap(0, nullptr);
    obj->vertexBufferView.BufferLocation = obj->vertexBuffer->GetGPUVirtualAddress();
    obj->vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
    obj->vertexBufferView.StrideInBytes = sizeof(VertexData);

    // インデックスバッファ作成
    size_t indexBufferSize = sizeof(uint32_t) * UINT(obj->indices.size());
    obj->indexBuffer = Dx12ResourceFactory::CreateBufferResource(device_, indexBufferSize);
    uint32_t* iData = nullptr;
    obj->indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&iData));
    std::memcpy(iData, obj->indices.data(), indexBufferSize);
    obj->indexBuffer->Unmap(0, nullptr);
    obj->indexBufferView.BufferLocation = obj->indexBuffer->GetGPUVirtualAddress();
    obj->indexBufferView.SizeInBytes = static_cast<UINT>(indexBufferSize);
    obj->indexBufferView.Format = DXGI_FORMAT_R32_UINT;

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
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath.c_str(),
        aiProcess_Triangulate               // 面を三角形に分割する
        | aiProcess_ConvertToLeftHanded     // 左手座標系に変換する(逆に言うとこのエンジンで使用するモデルは右手座標系で作成する必要がある)
        | aiProcess_GenSmoothNormals        // 法線データが存在しないときに自動生成する
        | aiProcess_JoinIdenticalVertices   // 重複頂点を結合する
    );
    assert(scene->HasMeshes());

    std::vector<VertexData> vertices;
    std::vector<uint32_t> indices;

    // メッシュ取得
    aiMesh* mesh = scene->mMeshes[0];

    // 各情報が存在するか
    const bool hasNormals = mesh->HasNormals();
    Log("法線データが存在しません。自動生成された数値が使用されます");
    const bool hasTexCoords = mesh->HasTextureCoords(0);
    Log("テクスチャ座標データが存在しません。(0.0f,0.0f)で初期化されます");

    vertices.resize(mesh->mNumVertices);
    for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
    {
        aiVector3D& position = mesh->mVertices[vertexIndex];
        vertices[vertexIndex].position = { position.x, position.y, position.z, 1.0f };

        if (hasNormals)
        {
            aiVector3D& normal = mesh->mNormals[vertexIndex];
            vertices[vertexIndex].normal = { normal.x, normal.y, normal.z };
        }
        else
        {
            vertices[vertexIndex].normal = { 0.0f, 1.0f, 0.0f };
        }
        if (hasTexCoords)
        {
            aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
            vertices[vertexIndex].texcoord = { texcoord.x, texcoord.y };
        }
        else
        {
            vertices[vertexIndex].texcoord = { 0.0f, 0.0f };
        }
    }

    indices.resize(mesh->mNumFaces * 3);
    for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
    {
        const aiFace& face = mesh->mFaces[faceIndex];
        assert(face.mNumIndices == 3); // 三角形でなければエラー

        indices[faceIndex * 3 + 0] = face.mIndices[0];
        indices[faceIndex * 3 + 1] = face.mIndices[1];
        indices[faceIndex * 3 + 2] = face.mIndices[2];
    }

    modelData->vertices = vertices;
    modelData->indices = indices;
    modelData->rootNode = ReadNode(scene->mRootNode);
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

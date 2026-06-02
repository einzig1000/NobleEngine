#include "ModelManager.h"
#include <Utilities/Logger/Logger.h>
#include <DirectX/Resource/Dx12ResourceFactory.h>
#include <filesystem> 
#include <fstream>


ModelManager::ModelManager(ID3D12Device* device)
	: device_(device)
{}

ModelManager::~ModelManager()
{}


int32_t ModelManager::LoadModel(const std::string& filePath)
{
    auto path = std::filesystem::path(filePath);

    // ディレクトリ名
    std::string directory = path.parent_path().string();
    // 拡張子を除いたファイル名
    std::string stem = path.stem().string();

    auto exists = std::find_if(
        objects.begin(), objects.end(),
        [&filePath](const ModelData& model) { return model.filePath == filePath; }
    );
    if (exists != objects.end())
    {
        return exists->number;
    }

    Log("モデル読み込み開始:%s", filePath.c_str());

    // ボックスを作成
    ModelData obj;
    // モデルデータ
    LoadModelFile(filePath, obj);
    // AABB .obj → .csv へ拡張子を変換して渡す
    std::string csvFilename = directory + "/" + stem + ".csv";
    obj.aabb = LoadAABB(csvFilename, obj.vertices);
    // 識別ナンバー
    obj.number = static_cast<uint32_t>(objects.size());
    // ファイルパス
    obj.filePath = filePath;

    // まず空のModelDataをvectorに追加し、参照を取得
    objects.push_back(obj);
    ModelData& ref = objects.back();

    // 頂点バッファ作成
	size_t vertexBufferSize = sizeof(VertexData) * ref.vertices.size();
    ref.vertexBuffer = Dx12ResourceFactory::CreateBufferResource(device_, vertexBufferSize);
    VertexData* vData = nullptr;
    ref.vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vData));
    std::memcpy(vData, ref.vertices.data(), vertexBufferSize);
    ref.vertexBuffer->Unmap(0, nullptr);
    ref.vertexBufferView.BufferLocation = ref.vertexBuffer->GetGPUVirtualAddress();
    ref.vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
    ref.vertexBufferView.StrideInBytes = sizeof(VertexData);

    // インデックスバッファ作成
    size_t indexBufferSize = sizeof(uint32_t) * UINT(ref.indices.size());
    ref.indexBuffer = Dx12ResourceFactory::CreateBufferResource(device_, indexBufferSize);
    uint32_t* iData = nullptr;
    ref.indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&iData));
    std::memcpy(iData, ref.indices.data(), indexBufferSize);
    ref.indexBuffer->Unmap(0, nullptr);
    ref.indexBufferView.BufferLocation = ref.indexBuffer->GetGPUVirtualAddress();
    ref.indexBufferView.SizeInBytes = static_cast<UINT>(indexBufferSize);
	ref.indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    Log("成功 ID:%d", ref.number);

    return ref.number;
}

int32_t ModelManager::CreateModel(const std::vector<VertexData>& vertices, const std::string& name)
{
    Log("モデル作成開始:%s", name.c_str());

    ModelData obj;
    obj.vertices = vertices;
    obj.aabb = LoadAABBFromCSV("default_aabb.csv"); // 仮のCSVファイル名
    obj.number = static_cast<uint32_t>(objects.size());
    obj.filePath = name;

    objects.push_back(obj);
    ModelData& ref = objects.back();

    // 頂点バッファ作成
    size_t vertexBufferSize = sizeof(VertexData) * ref.vertices.size();
    ref.vertexBuffer = Dx12ResourceFactory::CreateBufferResource(device_, vertexBufferSize);
    VertexData* vData = nullptr;
    ref.vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vData));
    std::memcpy(vData, ref.vertices.data(), vertexBufferSize);
    ref.vertexBuffer->Unmap(0, nullptr);

    ref.vertexBufferView.BufferLocation = ref.vertexBuffer->GetGPUVirtualAddress();
    ref.vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
    ref.vertexBufferView.StrideInBytes = sizeof(VertexData);

    Log("成功 ID:%d", ref.number);

    return ref.number;
}

ModelData* ModelManager::GetModelData(int32_t modelID)
{
    if (modelID < 0)
    {
        Log("存在しないモデルIDです:%d", modelID);
        return &objects[0];
    }

	if (modelID < objects.size())
	{
		return &objects[modelID];
	}
	else
	{
		Log("存在しないモデルIDです:%d", modelID);
		return nullptr;
	}
}



// AABB.csvの読み込み & 存在しなければ作成,保存
std::vector<AABB> ModelManager::LoadAABB(const std::string& filePath, const std::vector<VertexData>& vertices)
{
	std::vector<AABB> aabbs;
	if (std::filesystem::exists(filePath))
	{
		aabbs = LoadAABBFromCSV(filePath);
	}
	else
	{
		// 今までの方法でAABBを1つ作成
		AABB aabb = CreateLocalAABB(vertices);
		aabbs.push_back(aabb);
		SaveAABBToCSV(filePath, aabbs);
	}
	return aabbs;
}

// AABB.csvの読み込み
std::vector<AABB> ModelManager::LoadAABBFromCSV(const std::string& filePath)
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
AABB ModelManager::CreateLocalAABB(const std::vector<VertexData>& vertices)
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
void ModelManager::SaveAABBToCSV(const std::string& filePath, const std::vector<AABB>& aabbs)
{
    std::ofstream file(filePath);
    file << "min_x,min_y,min_z,max_x,max_y,max_z\n";
    for (const auto& aabb : aabbs)
    {
        file << aabb.min.x << "," << aabb.min.y << "," << aabb.min.z << ","
            << aabb.max.x << "," << aabb.max.y << "," << aabb.max.z << "\n";
    }
}


// mtlファイルを読み込む関数
MaterialData ModelManager::LoadMaterialTemplateFile(const std::string& filePath)
{
	MaterialData materialData;

    std::string line;

    // ファイルを開く
    std::ifstream file(filePath);
    assert(file.is_open());

    // MaterialDataを構築する
    while (std::getline(file, line))
    {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;

        // 
        if (identifier == "map_KD")
        {
            std::string textureFilename;
            s >> textureFilename;
        }
    }

    // 構築したMaterialDataをreturnする
	return materialData;
}

// モデルファイルを読み込む関数
void ModelManager::LoadModelFile(const std::string& filePath, ModelData& modelData)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath.c_str(),
        aiProcess_Triangulate             // 面を三角形に分割する
        | aiProcess_ConvertToLeftHanded     // 左手座標系に変換する(逆に言うとこのエンジンで使用するモデルは右手座標系で作成する必要がある)
        | aiProcess_GenSmoothNormals        // 法線データが存在しないときに自動生成する
        | aiProcess_JoinIdenticalVertices     // 重複頂点を結合する
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

    modelData.vertices = vertices;
    modelData.indices = indices;
    modelData.rootNode = ReadNode(scene->mRootNode);
}

Node ModelManager::ReadNode(const aiNode* node)
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

Skeleton ModelManager::CreateSkeleton(const Node& node)
{
	Skeleton skeleton;
    skeleton.root = CreateJoint(node, std::nullopt, skeleton.joints);

	for (size_t i = 0; i < skeleton.joints.size(); ++i)
	{
		skeleton.jointIndexByName[skeleton.joints[i].name] = int32_t(i);
	}

	return skeleton;
}

int32_t ModelManager::CreateJoint(const Node& node, const std::optional<int32_t>& parentIndex, std::vector<Joint>& joints)
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
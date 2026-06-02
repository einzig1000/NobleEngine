//#include "ModelLoader.h"
//#include <Utilities/Logger/Logger.h>
//#include <DirectX/Resource/Dx12ResourceFactory.h>
//#include <filesystem> 
//
//ModelLoader::ModelLoader(ID3D12Device* device)
//{}
//
//ModelLoader::~ModelLoader()
//{}
//
//int32_t ModelLoader::LoadModel(const std::string & filePath)
//{
//    auto path = std::filesystem::path(filePath);
//
//    // ディレクトリ名
//    std::string directory = path.parent_path().string();
//    // 拡張子を除いたファイル名
//    std::string stem = path.stem().string();
//
//    auto exists = std::find_if(
//        objects.begin(), objects.end(),
//        [&filePath](const ModelData& model) { return model.filePath == filePath; }
//    );
//    if (exists != objects.end())
//    {
//        return exists->number;
//    }
//
//    Log("モデル読み込み開始:%s", filePath.c_str());
//
//    // ボックスを作成
//    ModelData obj;
//    // モデルデータ
//    LoadModelFile(filePath, obj);
//    // AABB .obj → .csv へ拡張子を変換して渡す
//    std::string csvFilename = directory + "/" + stem + ".csv";
//    obj.aabb = LoadAABB(csvFilename, obj.vertices);
//    // 識別ナンバー
//    obj.number = static_cast<uint32_t>(objects.size());
//    // ファイルパス
//    obj.filePath = filePath;
//
//    // まず空のModelDataをvectorに追加し、参照を取得
//    objects.push_back(obj);
//    ModelData& ref = objects.back();
//
//    // 頂点バッファ作成
//    size_t vertexBufferSize = sizeof(VertexData) * ref.vertices.size();
//    ref.vertexBuffer = Dx12ResourceFactory::CreateBufferResource(device_, vertexBufferSize);
//    VertexData* vData = nullptr;
//    ref.vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vData));
//    std::memcpy(vData, ref.vertices.data(), vertexBufferSize);
//    ref.vertexBuffer->Unmap(0, nullptr);
//    ref.vertexBufferView.BufferLocation = ref.vertexBuffer->GetGPUVirtualAddress();
//    ref.vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
//    ref.vertexBufferView.StrideInBytes = sizeof(VertexData);
//
//    // インデックスバッファ作成
//    size_t indexBufferSize = sizeof(uint32_t) * UINT(ref.indices.size());
//    ref.indexBuffer = Dx12ResourceFactory::CreateBufferResource(device_, indexBufferSize);
//    uint32_t* iData = nullptr;
//    ref.indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&iData));
//    std::memcpy(iData, ref.indices.data(), indexBufferSize);
//    ref.indexBuffer->Unmap(0, nullptr);
//    ref.indexBufferView.BufferLocation = ref.indexBuffer->GetGPUVirtualAddress();
//    ref.indexBufferView.SizeInBytes = static_cast<UINT>(indexBufferSize);
//    ref.indexBufferView.Format = DXGI_FORMAT_R32_UINT;
//
//    Log("成功 ID:%d", ref.number);
//
//    return ref.number;
//}
//
//MaterialData ModelLoader::LoadMaterialTemplateFile(const std::string& filePath)
//{
//    return MaterialData();
//}
//
//void ModelLoader::LoadModelFile(const std::string& filePath, ModelData& modelData)
//{
//    Assimp::Importer importer;
//    const aiScene* scene = importer.ReadFile(filePath.c_str(),
//        aiProcess_Triangulate             // 面を三角形に分割する
//        | aiProcess_ConvertToLeftHanded     // 左手座標系に変換する(逆に言うとこのエンジンで使用するモデルは右手座標系で作成する必要がある)
//        | aiProcess_GenSmoothNormals        // 法線データが存在しないときに自動生成する
//        | aiProcess_JoinIdenticalVertices     // 重複頂点を結合する
//    );
//    assert(scene->HasMeshes());
//
//    std::vector<VertexData> vertices;
//    std::vector<uint32_t> indices;
//
//    // メッシュ取得
//    aiMesh* mesh = scene->mMeshes[0];
//
//    // 各情報が存在するか
//    const bool hasNormals = mesh->HasNormals();
//    Log("法線データが存在しません。自動生成された数値が使用されます");
//    const bool hasTexCoords = mesh->HasTextureCoords(0);
//    Log("テクスチャ座標データが存在しません。(0.0f,0.0f)で初期化されます");
//
//    vertices.resize(mesh->mNumVertices);
//    for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
//    {
//        aiVector3D& position = mesh->mVertices[vertexIndex];
//        vertices[vertexIndex].position = { position.x, position.y, position.z, 1.0f };
//
//        if (hasNormals)
//        {
//            aiVector3D& normal = mesh->mNormals[vertexIndex];
//            vertices[vertexIndex].normal = { normal.x, normal.y, normal.z };
//        }
//        else
//        {
//            vertices[vertexIndex].normal = { 0.0f, 1.0f, 0.0f };
//        }
//        if (hasTexCoords)
//        {
//            aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
//            vertices[vertexIndex].texcoord = { texcoord.x, texcoord.y };
//        }
//        else
//        {
//            vertices[vertexIndex].texcoord = { 0.0f, 0.0f };
//        }
//    }
//
//    indices.resize(mesh->mNumFaces * 3);
//    for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
//    {
//        const aiFace& face = mesh->mFaces[faceIndex];
//        assert(face.mNumIndices == 3); // 三角形でなければエラー
//
//        indices[faceIndex * 3 + 0] = face.mIndices[0];
//        indices[faceIndex * 3 + 1] = face.mIndices[1];
//        indices[faceIndex * 3 + 2] = face.mIndices[2];
//    }
//
//    modelData.vertices = vertices;
//    modelData.indices = indices;
//    modelData.rootNode = ReadNode(scene->mRootNode);
//}
//
//Node ModelLoader::ReadNode(const aiNode* node)
//{
//    Node result;
//    aiVector3D scale;
//    aiQuaternion rotate;
//    aiVector3D translate;
//    node->mTransformation.Decompose(scale, rotate, translate);
//    result.transform.scale = { scale.x, scale.y, scale.z };
//    result.transform.rotate = { rotate.x, -rotate.y, -rotate.z, rotate.w };
//    result.transform.translate = { -translate.x, translate.y, translate.z };
//    result.localMatrix = Matrix4x4::MakeAffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);
//    result.name = node->mName.C_Str();
//    result.children.resize(node->mNumChildren);
//    for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
//    {
//        result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
//    }
//    return result;
//}

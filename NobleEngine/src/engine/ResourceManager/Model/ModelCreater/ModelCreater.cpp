#include "ModelCreater.h"
#include <Utilities/Logger/Logger.h>
#include <DirectX/Resource/Dx12ResourceFactory.h>
#include <ResourceManager/Model/ModelBank/ModelBank.h>
#include <filesystem>

ModelCreater::ModelCreater(ID3D12Device2* device, ModelBank* bank)
	: device_(device), bank_(bank)
{}

ModelCreater::~ModelCreater()
{}

int32_t ModelCreater::CreateModel(const std::vector<VertexData>& vertices, const std::string& name)
{
    Log("モデル作成開始:%s", name.c_str());

    std::unique_ptr<ModelData> obj = std::make_unique<ModelData>();

    // AABB作成
    // ref->aabb = CreateLocalAABB(vertices);

	// 表記揺れを防ぐため小文字に変換してファイルパスとして扱う
    std::string filePath = name;
    std::transform(filePath.begin(), filePath.end(), filePath.begin(), ::tolower);
	obj->filePath = filePath;

    // 頂点バッファ作成
    obj->vertices = vertices;
    size_t vertexBufferSize = sizeof(VertexData) * obj->vertices.size();
    obj->vertexBuffer = Dx12ResourceFactory::CreateBufferResource(device_, vertexBufferSize);
    VertexData* vData = nullptr;
    obj->vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vData));
    std::memcpy(vData, obj->vertices.data(), vertexBufferSize);
    obj->vertexBuffer->Unmap(0, nullptr);
    obj->vertexBufferView.BufferLocation = obj->vertexBuffer->GetGPUVirtualAddress();
    obj->vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
    obj->vertexBufferView.StrideInBytes = sizeof(VertexData);

    // モデルバンクに登録
    int32_t ID = bank_->AllocateModelID();
    bank_->AddModelData(filePath, ID, std::move(obj));

    Log("成功 ID:%d", ID);

    return ID;
}

#include "ModelCreater.h"
#include <Utilities/Logger/Logger.h>
#include <DirectX/ResourceUtilities/ResourceUtilities.h>
#include <DirectX/DirectXManager.h>
#include <AssetManager/Model/ModelBank/ModelBank.h>
#include <AssetManager/Model/ModelHelper/ModelHelper.h>
#include <filesystem>
#include <externals/meshoptimizer-1.1/meshoptimizer.h>

ModelCreater::ModelCreater(DirectXManager* dxManager, ModelBank* bank)
	: dxManager_(dxManager), bank_(bank)
{}

ModelCreater::~ModelCreater()
{
	intermediateUploadResources_.clear();
}

int32_t ModelCreater::CreateModel(const std::vector<VertexData>& vertices, const std::string& name, const bool optimize)
{
	if (vertices.empty()) return -1;

    Log("モデル作成開始:%s", name.c_str());

    uint32_t backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
    auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);
    auto* srvManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();

    std::unique_ptr<ModelData> modelData = std::make_unique<ModelData>();

    // 頂点データの取得
    modelData->vertices = vertices; 

	// 形だけ作成
    std::vector<uint32_t> indices(vertices.size());
    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        indices[i] = i;
    }
    modelData->indices = indices;

    // 頂点データ & インデックスデータの最適化
	if (optimize)
    {
        std::vector<uint32_t> remap(modelData->vertices.size());
    
        size_t vertexCount = meshopt_generateVertexRemap(
            remap.data(),
            modelData->indices.data(),
            modelData->indices.size(),
            modelData->vertices.data(),
            modelData->vertices.size(),
            sizeof(VertexData));

        //std::vector<VertexData> vertices(vertexCount);
		std::vector<VertexData> optimizedVertices(vertexCount);
        std::vector<uint32_t> indices(modelData->indices.size());
    
        // インデックスバッファをリマップ
        meshopt_remapIndexBuffer(
            indices.data(),
            modelData->indices.data(),
            modelData->indices.size(),
            remap.data());
    
        // 頂点データをリマップ
        meshopt_remapVertexBuffer(
            optimizedVertices.data(),
            modelData->vertices.data(),
            modelData->vertices.size(),
            sizeof(VertexData),
            remap.data());
    
        // 最適なサイズに圧縮
        modelData->vertices.resize(optimizedVertices.size());
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
            &optimizedVertices[0].position.x,
            optimizedVertices.size(),
            sizeof(VertexData),
            1.05f);
    
        // 頂点フェッチ最適化
        meshopt_optimizeVertexFetch(
            modelData->vertices.data(),
            modelData->indices.data(),
            modelData->indices.size(),
            optimizedVertices.data(),
            optimizedVertices.size(),
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
                tris =  (static_cast<uint32_t>(meshletTriangles[baseIndex]) |
                        (static_cast<uint32_t>(meshletTriangles[baseIndex + 1]) << 10) |
                        (static_cast<uint32_t>(meshletTriangles[baseIndex + 2]) << 20));
                modelData->primitiveIndices.push_back(tris);
            }

            meshlet.vertex_offset = vertexOffset;
            meshlet.triangle_offset = primitiveOffset;

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

    // デフォルトコライダー作成
	modelData->colliderShape.aabbs.push_back(ModelHelper::CreateDefaultAABB(modelData->vertices));

	// 表記揺れを防ぐため小文字に変換してファイルパスとして扱う
    std::string filePath = name;
    std::transform(filePath.begin(), filePath.end(), filePath.begin(), ::tolower);
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


    // モデルバンクに登録
    int32_t ID = bank_->AddModelData(filePath, std::move(modelData));

    Log("成功 ID:%d", ID);

    return ID;
}

#include "ResourceManager.h"

ResourceManager::ResourceManager(ID3D12GraphicsCommandList* commandList, DescriptorHeapManager* descriptorHeap, ID3D12Device* device)
{
    audioManager_ = std::make_unique<AudioManager>();
    textureManager_ = std::make_unique<TextureManager>(commandList, descriptorHeap, device);
    modelManager_ = std::make_unique<ModelManager>(device);
}

ResourceManager::~ResourceManager()
{}

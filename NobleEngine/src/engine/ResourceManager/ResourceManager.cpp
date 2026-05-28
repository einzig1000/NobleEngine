#include "ResourceManager.h"
#include <DirectX/DirectXManager.h>

ResourceManager::ResourceManager(DirectXManager* dxManager)
{
    audioManager_ = std::make_unique<AudioManager>();
    textureManager_ = std::make_unique<TextureManager>(dxManager);
    modelManager_ = std::make_unique<ModelManager>(dxManager->GetDevice());
}

ResourceManager::~ResourceManager()
{}

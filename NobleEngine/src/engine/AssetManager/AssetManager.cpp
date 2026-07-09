#include "AssetManager.h"
#include <DirectX/DirectXManager.h>
#include "AssetManager.h"

AssetManager::AssetManager(DirectXManager* dxManager)
{
    audioManager_ = std::make_unique<AudioManager>();
    textureManager_ = std::make_unique<TextureManager>(dxManager);
    modelManager_ = std::make_unique<ModelManager>(dxManager);
}

AssetManager::~AssetManager()
{}

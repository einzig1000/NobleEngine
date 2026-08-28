#include "AssetManager.h"

AssetManager::AssetManager(DirectXManager* dxManager)
{
    audioManager_ = std::make_unique<AudioManager>();
    textureManager_ = std::make_unique<TextureManager>(dxManager);
    modelManager_ = std::make_unique<ModelManager>(dxManager);
	animationManager_ = std::make_unique<AnimationManager>(dxManager);
	fontManager_ = std::make_unique<FontManager>(dxManager, modelManager_.get());
}

AssetManager::~AssetManager()
{}

void AssetManager::Update()
{
    audioManager_->Update();
}
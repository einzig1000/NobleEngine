#pragma once
#include <AssetManager/Audio/AudioManager.h>
#include <AssetManager/Texture/TextureManager.h>
#include <AssetManager/Model/ModelManager.h>
#include <memory> 

class DirectXManager;

/// <summary>
/// リソース管理クラス
/// </summary>
class AssetManager
{
public:
	AssetManager(DirectXManager* dxManager);
	~AssetManager();

	AudioManager* GetAudioManager() const { return audioManager_.get(); }
	TextureManager* GetTextureManager() const { return textureManager_.get(); }
	ModelManager* GetModelManager() const { return modelManager_.get(); }

private:

	std::unique_ptr<AudioManager> audioManager_;
	std::unique_ptr<TextureManager> textureManager_;
	std::unique_ptr<ModelManager> modelManager_;

};


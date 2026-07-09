#pragma once
#include "Audio/AudioManager.h"
#include "Texture/TextureManager.h"
#include "Model/ModelManager.h"
#include "Animation/AnimationManager.h"
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
	AnimationManager* GetAnimationManager() const { return animationManager_.get(); }

private:

	std::unique_ptr<AudioManager> audioManager_;
	std::unique_ptr<TextureManager> textureManager_;
	std::unique_ptr<ModelManager> modelManager_;
	std::unique_ptr<AnimationManager> animationManager_;

};


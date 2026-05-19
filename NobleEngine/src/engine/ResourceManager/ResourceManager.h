#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "ResourceManager/Audio/AudioManager.h"
#include "ResourceManager/Texture/TextureManager.h"
#include "ResourceManager/Model/ModelManager.h"
#include <memory> 

/// <summary>
/// リソース管理クラス
/// </summary>
class ResourceManager
{
public:
	ResourceManager(ID3D12GraphicsCommandList* commandList, DescriptorHeapManager* descriptorHeap, ID3D12Device* device);
	~ResourceManager();


	AudioManager* GetAudioManager() const { return audioManager_.get(); }
	TextureManager* GetTextureManager() const { return textureManager_.get(); }
	ModelManager* GetModelManager() const { return modelManager_.get(); }

private:

	std::unique_ptr<AudioManager> audioManager_;
	std::unique_ptr<TextureManager> textureManager_;
	std::unique_ptr<ModelManager> modelManager_;

};


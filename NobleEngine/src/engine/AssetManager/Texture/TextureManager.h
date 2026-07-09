#pragma once
#include "TextureLoader/TextureLoader.h"
#include "TextureBank/TextureBank.h"
#include <memory>

class DirectXManager;

/// <summary>
/// テクスチャ管理クラス
/// </summary>
class TextureManager
{
public:
    TextureManager(DirectXManager* dxManager);
    ~TextureManager();

	TextureLoader* GetTextureLoader() const { return loader_.get(); }
	TextureBank* GetTextureBank() const { return bank_.get(); }

private:
	std::unique_ptr<TextureLoader> loader_;
	std::unique_ptr<TextureBank> bank_;
};


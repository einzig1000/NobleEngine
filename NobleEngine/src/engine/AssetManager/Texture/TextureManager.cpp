#include "TextureManager.h"
#include <DirectX/DirectXManager.h>

TextureManager::TextureManager(DirectXManager* dxManager)
{
    // テクスチャバンク作成
    bank_ = std::make_unique<TextureBank>();
	// テクスチャローダー作成
	loader_ = std::make_unique<TextureLoader>(dxManager, bank_.get());
}

TextureManager::~TextureManager()
{
}

#include "TextureManager.h"
#include <externals/DirectXTex/d3dx12.h>
#include <externals/DirectXTex/DirectXTex.h>
#include <Utilities/Logger/Logger.h>
#include <DirectX/Resource/Dx12ResourceFactory.h>
#include <DirectX/DirectXManager.h>
#include <definition/definition.h>
#include <filesystem>
#include <cassert>
#include <cctype>
#include <algorithm>

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

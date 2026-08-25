#include "TextureManager.h"
#include <Utilities/Logger/Logger.h>

TextureManager::TextureManager(DirectXManager* dxManager)
{
    Log("コンストラクタ実行開始 : TextureManager");

    // テクスチャバンク作成
    bank_ = std::make_unique<TextureBank>();
	// テクスチャローダー作成
	loader_ = std::make_unique<TextureLoader>(dxManager, bank_.get());

    Log("成功");
}

TextureManager::~TextureManager()
{
    Log("デストラクタ実行成功 : TextureManager");
}

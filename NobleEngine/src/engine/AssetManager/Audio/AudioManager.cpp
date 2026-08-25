#include "AudioManager.h"
#include <Utilities/Logger/Logger.h>

// x
AudioManager::AudioManager()
{
    Log("コンストラクタ実行開始 : AudioManager");

	bank_ = std::make_unique<AudioBank>();
	player_ = std::make_unique<AudioPlayer>(bank_.get());
	loader_ = std::make_unique<AudioLoader>(bank_.get(), player_->GetXAudio2());

	Log("成功");
}

// x
AudioManager::~AudioManager()
{
    Log("デストラクタ実行成功 : AudioManager");
}

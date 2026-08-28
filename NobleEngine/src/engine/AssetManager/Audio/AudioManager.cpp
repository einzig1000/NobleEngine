#include "AudioManager.h"
#include <Utilities/Logger/Logger.h>

AudioManager::AudioManager()
{
    Log("コンストラクタ実行開始 : AudioManager");

	bank_ = std::make_unique<AudioBank>();
	player_ = std::make_unique<AudioPlayer>(bank_.get());
	loader_ = std::make_unique<AudioLoader>(bank_.get());

	Log("成功");
}

AudioManager::~AudioManager()
{
    Log("デストラクタ実行成功 : AudioManager");
}

void AudioManager::Update()
{
	player_->Update();
}
#pragma once
#include "AudioLoader/AudioLoader.h"
#include "AudioPlayer/AudioPlayer.h"
#include "AudioBank/AudioBank.h"
#include <memory>

/// <summary>
/// オーディオ管理クラス
/// </summary>
class AudioManager
{
public:
    AudioManager();
    ~AudioManager();
	void Update();

	AudioLoader* GetAudioLoader() const { return loader_.get(); }
	AudioPlayer* GetAudioPlayer() const { return player_.get(); }
	AudioBank* GetAudioBank() const { return bank_.get(); }

private:
	std::unique_ptr<AudioLoader> loader_;
	std::unique_ptr<AudioPlayer> player_;
	std::unique_ptr<AudioBank> bank_;

};
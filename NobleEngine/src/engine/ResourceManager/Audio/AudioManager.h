#pragma once
#include <ResourceManager/Audio/AudioLoader/AudioLoader.h>
#include <ResourceManager/Audio/AudioPlayer/AudioPlayer.h>
#include <ResourceManager/Audio/AudioBank/AudioBank.h>
#include <memory>

/// <summary>
/// オーディオ管理クラス
/// </summary>
class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

	AudioLoader* GetAudioLoader() const { return loader_.get(); }
	AudioPlayer* GetAudioPlayer() const { return player_.get(); }
	AudioBank* GetAudioBank() const { return bank_.get(); }

private:
	std::unique_ptr<AudioLoader> loader_;
	std::unique_ptr<AudioPlayer> player_;
	std::unique_ptr<AudioBank> bank_;

};
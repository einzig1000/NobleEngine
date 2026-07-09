#pragma once
#include <EngineDefinition/EngineDefinition.h>

class AudioBank;

class AudioPlayer
{
public:
    AudioPlayer(AudioBank* bank);
	~AudioPlayer();

    // 読み込まれたオーディオを再生
    void PlayAudio(const int32_t& audioId, bool loop);

    // 特定のオーディオの再生を停止
    void StopAudio(const int32_t& audioId);

    // 特定のオーディオまたはマスターボリュームを設定
    void SetVolume(const int32_t& audioId, float volume);
    void SetMasterVolume(float volume);

    // 特定のオーディオまたはマスターボリュームを返す
    float GetVolume(const int32_t& audioId);
    float GetMasterVolume();

    // 現在再生してるか？
    bool IsAudioPlaying(const int32_t& audioId);

    // IXAudio2エンジンのポインタを取得できるようにする
    IXAudio2* GetXAudio2() const { return pXAudio2.Get(); }

private:
	AudioBank* bank_;
    Microsoft::WRL::ComPtr<IXAudio2> pXAudio2;
    IXAudio2MasteringVoice* pMasteringVoice;
};


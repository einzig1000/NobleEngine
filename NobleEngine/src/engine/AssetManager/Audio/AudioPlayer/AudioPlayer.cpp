
#include "AudioPlayer.h"
#include <Utilities/Logger/Logger.h>
#include <AssetManager/Audio/AudioBank/AudioBank.h>
#include <algorithm>
#include <mfapi.h>

AudioPlayer::AudioPlayer(AudioBank* bank)
	: bank_(bank)
{
    // XAudio2の初期化
    HRESULT hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr))
    {
        Log("XAudio2エンジンの作成に失敗しました。hr = %s", HrToString(hr));
        assert(0);
    }

    // マスタリングボイスの作成
    hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice);
    if (FAILED(hr))
    {
        Log("XAudio2マスタリングボイスの作成に失敗しました。hr = %s", HrToString(hr));
        assert(0);
    }

    // Media Foundationの初期化
    // Media Foundation APIを使用する前にMFStartupを呼び出す必要があります
    hr = MFStartup(MF_VERSION, MFSTARTUP_FULL);
    if (FAILED(hr))
    {
        Log("Media Foundationの初期化に失敗しました。HRESULT: 0x%X", hr);
        if (pMasteringVoice) pMasteringVoice->DestroyVoice();
        pMasteringVoice = nullptr;
        assert(0);
    }
}

AudioPlayer::~AudioPlayer()
{
    // 再生中のボイスを破棄
    for (auto& pair : activeVoices_)
    {
        pair.second->Stop(0);
        pair.second->FlushSourceBuffers();
        pair.second->DestroyVoice();
    }
    activeVoices_.clear();

    // マスタリングボイスを破棄
    if (pMasteringVoice)
    {
        pMasteringVoice->DestroyVoice();
        pMasteringVoice = nullptr;
    }
    pXAudio2.Reset();

    // Media Foundationを終了
    MFShutdown();
}

void AudioPlayer::Update()
{
    for (auto it = activeVoices_.begin(); it != activeVoices_.end(); )
    {
        XAUDIO2_VOICE_STATE state;
        it->second->GetState(&state);

        if (state.BuffersQueued == 0)
        {
            it->second->DestroyVoice();
            it = activeVoices_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}


int32_t AudioPlayer::PlayAudio(const int32_t& audioId, bool loop)
{
    const AudioData* audioData = bank_->GetAudioData(audioId);
    if (!audioData)
    {
        Log("存在しないオーディオの再生を失敗しました。ID: %u", audioId);
        assert(0);
        return -1;
    }

    // この再生専用のボイスを新規作成する。原本(audioData)は一切書き換えない
    IXAudio2SourceVoice* voice = nullptr;
    HRESULT hr = pXAudio2->CreateSourceVoice(&voice, audioData->pWfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr);
    if (FAILED(hr))
    {
		Log("Voiceの作成に失敗しました。ID: %u HRESULT: 0x%X", audioId, hr);
        assert(0);
        return -1;
    }

    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = audioData->audioBytes;
    buffer.pAudioData = audioData->audioData.data();
    buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;

    hr = voice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr))
    {
		Log("オーディオのバッファ送信に失敗しました。 ID: %u HRESULT: 0x%X", audioId, hr);
        voice->DestroyVoice();
        assert(0);
        return -1;
    }

    hr = voice->Start(0);
    if (FAILED(hr))
    {
        Log("ID:%u のオーディオの再生に失敗しました。HRESULT: 0x%X", audioId, hr);
        voice->DestroyVoice();
        assert(0);
        return -1;
    }

    int32_t playId = nextPlayId_++;
    activeVoices_[playId] = voice;

    Log("ID:%u のオーディオを再生します。playId:%d Loop:%d", audioId, playId, loop);
    return playId;
}

void AudioPlayer::StopAudio(const int32_t& playId)
{
    auto it = activeVoices_.find(playId);
    if (it == activeVoices_.end())
    {
        Log("playId:%d のオーディオは既に再生終了しています。", playId);
        return;
    }

    it->second->Stop(0);
    it->second->FlushSourceBuffers();
    it->second->DestroyVoice();
    activeVoices_.erase(it);
    Log("playId:%d のオーディオを停止します。", playId);
}

void AudioPlayer::SetVolume(const int32_t& playId, float volume)
{
    auto it = activeVoices_.find(playId);
    if (it == activeVoices_.end())
    {
        Log("playId:%d のオーディオは既に再生終了しています。", playId);
        return;
    }
    float clampedVolume = std::clamp(volume, 0.0f, 1.0f);
    it->second->SetVolume(clampedVolume);
    Log("playId:%d のオーディオの音量を %f に設定します。", playId, clampedVolume);
}
float AudioPlayer::GetVolume(const int32_t& playId)
{
    auto it = activeVoices_.find(playId);
    if (it == activeVoices_.end())
    {
        Log("playId:%d のオーディオは既に再生終了しています。", playId);
        return 0.0f;
    }
    float currentVolume = 0.0f;
    it->second->GetVolume(&currentVolume);
    return currentVolume;
}

void AudioPlayer::SetMasterVolume(float volume)
{
    if (pMasteringVoice)
    {
        if (volume != GetMasterVolume())
        {
            float clampedVolume = std::max(0.0f, std::min(1.0f, volume));
            pMasteringVoice->SetVolume(clampedVolume);
            Log("マスターボリューム: %f", clampedVolume);
        }
    }
    else
    {
        Log("多分マスターボリュームが初期化されてないとかです");
        assert(0);
    }
}
float AudioPlayer::GetMasterVolume()
{
    if (pMasteringVoice)
    {
        float currentVolume = 0.0f;
        pMasteringVoice->GetVolume(&currentVolume);
        return currentVolume;
    }
    Log("マスター音量を取得できませんでした。");
    return 0.0f;
}

bool AudioPlayer::IsAudioPlaying(const int32_t& playId)
{
    auto it = activeVoices_.find(playId);
    if (it == activeVoices_.end())
    {
        return false;
    }

    XAUDIO2_VOICE_STATE state;
    it->second->GetState(&state);
    return state.BuffersQueued > 0;
}
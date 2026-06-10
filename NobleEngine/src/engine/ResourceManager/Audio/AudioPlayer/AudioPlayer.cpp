
#include "AudioPlayer.h"
#include <Utilities/Logger/Logger.h>
#include <ResourceManager/Audio/AudioBank/AudioBank.h>
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

void AudioPlayer::PlayAudio(const int32_t& audioId, bool loop)
{
	AudioData* audioData = bank_->GetAudioData(audioId);

    if (audioData)
    {
        // すでに再生中であれば停止
        audioData->pSourceVoice->Stop(0);
        audioData->pSourceVoice->FlushSourceBuffers(); // バッファをクリア

        // loopが１の場合XAUDIO2_LOOP_INFINITE(無限ループ)に設定。0なら0
        audioData->xAudioBuffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0; // ループ設定

        // オーディオデータをキューに送信し再生の準備をする(サブミット)
        HRESULT hr = audioData->pSourceVoice->SubmitSourceBuffer(&audioData->xAudioBuffer);
        if (FAILED(hr))
        {
            Log("Failed to submit source buffer for audio ID: %u HRESULT: 0x%X", audioId, hr);
            assert(0);
        }

        // 再生
        hr = audioData->pSourceVoice->Start(0);
        if (FAILED(hr))
        {
            Log("ID:%u のオーディオの再生に失敗しました。HRESULT: 0x%X", audioId, hr);
            assert(0);
        }
        Log("ID:%u のオーディオを再生します。Loop: %d", audioId, loop);
    }
    else
    {
        Log("存在しないオーディオの再生を失敗しました。ID: %u", audioId);
        assert(0);
    }
}

void AudioPlayer::StopAudio(const int32_t & audioId)
{
	AudioData* audioData = bank_->GetAudioData(audioId);
	if (audioData)
	{
		audioData->pSourceVoice->Stop(0);
		audioData->pSourceVoice->FlushSourceBuffers(); // バッファをクリア
		Log("ID:%u のオーディオを停止します。", audioId);
	}
	else
	{
		Log("存在しないオーディオの停止を失敗しました。ID: %u", audioId);
		assert(0);
	}
}

void AudioPlayer::SetVolume(const int32_t & audioId, float volume)
{
	AudioData* audioData = bank_->GetAudioData(audioId);
	if (audioData)
	{
		float clampedVolume = std::clamp(volume, 0.0f, 1.0f);
		audioData->pSourceVoice->SetVolume(clampedVolume);
		Log("ID:%u のオーディオの音量を %f に設定します。", audioId, clampedVolume);
	}
	else
	{
		Log("存在しないオーディオの音量設定を失敗しました。ID: %u", audioId);
		assert(0);
	}
}
float AudioPlayer::GetVolume(const int32_t & audioId)
{
	AudioData* audioData = bank_->GetAudioData(audioId);
	if (audioData)
    {
        float currentVolume = 0.0f;
        audioData->pSourceVoice->GetVolume(&currentVolume);
        return currentVolume;
    }
	Log("存在しないオーディオの音量取得を失敗しました。ID: %u", audioId);
    return 0.0f;
}

void AudioPlayer::SetMasterVolume(float volume)
{
    if (pMasteringVoice)
    {
        if (volume != GetMasterVolume())
        {
            float clampedVolume = my_max(0.0f, my_min(1.0f, volume));
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

bool AudioPlayer::IsAudioPlaying(const int32_t& audioId)
{
	AudioData* audioData = bank_->GetAudioData(audioId);

	if (audioData)
    {
        if (audioData->pSourceVoice)
        {
            XAUDIO2_VOICE_STATE state;
            audioData->pSourceVoice->GetState(&state);

            // キューにバッファがある場合、再生中または再生待ちと判断
            if (state.BuffersQueued > 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            Log("このIDのオーディンは存在しません", audioId);
            return false;
        }
    }
    else
    {
        Log("このIDのオーディンは存在しません", audioId);
        return false;
    }
}

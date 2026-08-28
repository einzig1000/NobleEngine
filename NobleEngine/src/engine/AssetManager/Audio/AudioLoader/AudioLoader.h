#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <xaudio2.h>

class AudioBank;

// XAudio2ボイスイベント用のカスタムコールバック
class VoiceCallback : public IXAudio2VoiceCallback
{
public:
    // バッファの再生が終了したときに呼び出される
    STDMETHOD_(void, OnBufferEnd)(void* pBufferContext) override {};
    // ボイスの処理パスが開始したときに呼び出される
    STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32 BytesRequired) override {}
    // ボイスの処理パスが終了したときに呼び出される
    STDMETHOD_(void, OnVoiceProcessingPassEnd)() override {}
    // ストリームが終了したときに呼び出される（ループ再生時等）
    STDMETHOD_(void, OnStreamEnd)() override {}
    // バッファの再生が開始したときに呼び出される
    STDMETHOD_(void, OnBufferStart)(void* pBufferContext) override {}
    // ループの終わりに達したときに呼び出される
    STDMETHOD_(void, OnLoopEnd)(void* pBufferContext) override {}
    // ボイスでエラーが発生したときに呼び出される
    STDMETHOD_(void, OnVoiceError)(void* pBufferContext, HRESULT Error) override {}
};

class AudioLoader
{
public:
    AudioLoader(AudioBank* bank);
    ~AudioLoader();

    // オーディオ読み込み
    int32_t LoadAudio(const std::string& filePath);

private:
	AudioBank* bank_;

    VoiceCallback voiceCallback;

    // VoiceCallbackからAudioManagerへのアクセスを許可
    friend class VoiceCallback;
};


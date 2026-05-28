#pragma once
#include <definition/definition.h>

#include <xaudio2.h>
#include <wrl/client.h> 

#include <vector>
#include <map>
#include <string>



// XAudio2ボイスイベント用のカスタムコールバック
class VoiceCallback : public IXAudio2VoiceCallback
{
public:
    // バッファの再生が終了したときに呼び出される
    STDMETHOD_(void, OnBufferEnd)(void* pBufferContext) override;
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

/// <summary>
/// オーディオ管理クラス
/// </summary>
class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

	// オーディオ読み込み
    int32_t LoadAudio(const std::string& filePath);

	// データ取得
	AudioData* GetAudioData(int32_t audioId);

	// オーディオ数を取得
	size_t GetAudioCount() const { return loadedAudio.size(); }

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

private:
    std::map<int32_t, AudioData> loadedAudio;

    // 初期化
    HRESULT Initialize();
    // デストラクタで何回もつかう
    void CleanupAudioData(AudioData& entry);

    Microsoft::WRL::ComPtr<IXAudio2> pXAudio2;
    IXAudio2MasteringVoice* pMasteringVoice;
    VoiceCallback voiceCallback;


    // VoiceCallbackからAudioManagerへのアクセスを許可
    friend class VoiceCallback;
};
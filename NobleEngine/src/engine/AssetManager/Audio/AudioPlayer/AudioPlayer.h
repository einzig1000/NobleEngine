#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <unordered_map>

class AudioBank;

class AudioPlayer
{
public:
    AudioPlayer(AudioBank* bank);
    ~AudioPlayer();

    // 再生完了したインスタンスの後始末。毎フレーム呼び出すこと
    void Update();

    /// <summary>
    /// オーディオ再生
    /// </summary>
    /// <param name="audioId">オーディオID(LoadAudioで取得)</param>
    /// <param name="loop">ループ再生するか否か</param>
    /// <returns>再生インスタンスID、失敗時は-1</returns>
    int32_t PlayAudio(const int32_t& audioId, bool loop);

    /// <summary>
    /// 再生停止
    /// </summary>
    /// <param name="playId">再生インスタンスID</param>
    void StopAudio(const int32_t& playId);


    /// <summary>
	/// 音量設定
    /// </summary>
    /// <param name="playId">再生インスタンスID</param>
    /// <param name="volume">音量 (0.0f~1.0f)</param>
    void SetVolume(const int32_t& playId, float volume);

    /// <summary>
	/// マスターボリューム設定
    /// </summary>
    /// <param name="volume">音量 (0.0f~1.0f)</param>
    void SetMasterVolume(float volume);

    /// <summary>
	/// 音量取得
    /// </summary>
    /// <param name="playId">再生インスタンスID</param>
    /// <returns>音量 (0.0f~1.0f)</returns>
    float GetVolume(const int32_t& playId);

	/// <summary>
	/// マスターボリューム取得
	/// </summary>
	/// <returns>音量 (0.0f~1.0f)</returns>
    float GetMasterVolume();

    /// <summary>
	/// 再生中か？
    /// </summary>
    /// <param name="playId">再生インスタンスID</param>
    /// <returns>再生中か？</returns>
    bool IsAudioPlaying(const int32_t& playId);

    // IXAudio2エンジンのポインタを取得できるようにする
    IXAudio2* GetXAudio2() const { return pXAudio2.Get(); }

private:
    AudioBank* bank_;
    Microsoft::WRL::ComPtr<IXAudio2> pXAudio2;
    IXAudio2MasteringVoice* pMasteringVoice;

    // playId -> 再生中のボイス
    std::unordered_map<int32_t, IXAudio2SourceVoice*> activeVoices_;
    int32_t nextPlayId_ = 0;
};
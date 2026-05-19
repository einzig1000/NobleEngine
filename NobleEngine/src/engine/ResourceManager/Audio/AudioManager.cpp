#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

#include <ResourceManager/Audio/AudioManager.h>
#include <Windows.h>
#include <string>
#include <Utilities/Logger/Logger.h>
#include <Utilities/Converter/StringConverter/StringConverter.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <mfreadwrite.h>
#include <atomic>
#include <sdkddkver.h>



// x
AudioManager::AudioManager()
    : pXAudio2(nullptr), pMasteringVoice(nullptr)
{
    Initialize();
    Log("コンストラクタ実行成功 : AudioManager");
}

// x
AudioManager::~AudioManager()
{
    for (auto& pair : loadedAudio)
    {
        CleanupAudioData(pair.second);
    }
    loadedAudio.clear();

    // マスタリングボイスを破棄
    if (pMasteringVoice)
    {
        pMasteringVoice->DestroyVoice();
        pMasteringVoice = nullptr;
    }
    pXAudio2.Reset();

    // Media Foundationを終了
    MFShutdown();
    Log("デストラクタ実行成功 : AudioManager");
}

// x
HRESULT AudioManager::Initialize()
{
    // XAudio2の初期化
    HRESULT hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr))
    {
        Log("XAudio2エンジンの作成に失敗しました。hr = %s", HrToString(hr));
        assert(0);
        return hr;
    }

    // マスタリングボイスの作成
    hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice);
    if (FAILED(hr))
    {
        Log("XAudio2マスタリングボイスの作成に失敗しました。hr = %s", HrToString(hr));
        assert(0);
        return hr;
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
        return hr;
    }

    return S_OK;
}

// 読み込み
uint32_t AudioManager::LoadAudio(const std::string& filePath)
{
	Log("オーディオ読み込み開始 パス:%s", filePath.c_str());

    static uint32_t nextAudioId = 0;
    AudioData entry = {};
    HRESULT hr = S_OK;

    // ファイルパスをワイド文字列に変換
    std::wstring wFilePath = StringConverter::Convert(filePath);

    Microsoft::WRL::ComPtr<IMFSourceReader> pSourceReader;


    // ソースリーダー(オーディオデータを読み取るためのインターフェース)の作成
    hr = MFCreateSourceReaderFromURL(wFilePath.c_str(), nullptr, &pSourceReader);
    if (FAILED(hr)) { Log("ソースリーダーの作成に失敗: %s", HrToString(hr)); assert(0); return UINT32_MAX; }

    // メディアファイルには 複数のストリーム（音声・動画・字幕など） が含まれていることがあるため音声を取得するよと設定しているらしい
    hr = pSourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
    if (FAILED(hr)) { Log("取得ストリームの設定に失敗: %s", HrToString(hr)); assert(0); return UINT32_MAX; }

    // Media Foundation に対して、オーディオストリームをPCM形式にデコードするように要求
    Microsoft::WRL::ComPtr<IMFMediaType> pOutputMediaType;
    hr = MFCreateMediaType(&pOutputMediaType);
    if (FAILED(hr)) { Log("PCM出力MFMediaTypeの作成に失敗: %s", HrToString(hr)); assert(0); return UINT32_MAX; }


    hr = pOutputMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    if (FAILED(hr)) { Log("PCM出力の主要タイプ設定に失敗: %s", HrToString(hr)); assert(0); return UINT32_MAX; }

    hr = pOutputMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    if (FAILED(hr)) { Log("PCM出力のサブタイプ設定に失敗: %s", HrToString(hr)); assert(0); return UINT32_MAX; }

    // 音声データがどんな形式(MP3,WAV,AACとか)で保存されているかを調べる
    hr = pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pOutputMediaType.Get());
    if (FAILED(hr)) { Log("出力タイプをPCMに設定できませんでした: %s", HrToString(hr)); assert(0); return UINT32_MAX; }

    // Media FoundationがPCMフォーマットに変えたはずなので確認
    Microsoft::WRL::ComPtr<IMFMediaType> pActualMediaType;
    hr = pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pActualMediaType);
    if (FAILED(hr)) { Log("実際のメディアタイプ取得に失敗: %s", HrToString(hr)); assert(0); return UINT32_MAX; }

    UINT32 formatSize = 0;
    WAVEFORMATEX* wfx = nullptr;
    hr = MFCreateWaveFormatExFromMFMediaType(pActualMediaType.Get(), &wfx, &formatSize, 0);
    if (FAILED(hr)) { Log("実際のメディアタイプのWAVEFORMATEX変換に失敗: %s", HrToString(hr)); assert(0); return UINT32_MAX; }
    entry.pWfx = wfx;
    entry.wfxSize = formatSize;



    //Log("--- WAVEFORMATEX Debug Info ---");
    //Log("wFormatTag: 0x%X", entry.pWfx->wFormatTag);
    //Log("nChannels: %u", entry.pWfx->nChannels);
    //Log("nSamplesPerSec: %u", entry.pWfx->nSamplesPerSec);
    //Log("nAvgBytesPerSec: %u", entry.pWfx->nAvgBytesPerSec);
    //Log("nBlockAlign: %u", entry.pWfx->nBlockAlign);
    //Log("wBitsPerSample: %u", entry.pWfx->wBitsPerSample);
    //Log("cbSize: %u", entry.pWfx->cbSize);
    //Log("-------------------------------");


    // オーディオデータの読み込み
    DWORD currentBufferLength = 0;
    DWORD totalAudioDataSize = 0; // 実際に読み込んだ総バイト数を追跡

    // 初期化時にいい感じの容量を予約 。必要に応じて拡張する
    entry.audioData.reserve(1024 * 1024); // 1 MB を予約

    while (true)
    {
        Microsoft::WRL::ComPtr<IMFSample> pSample;
        DWORD actualStreamIndex = 0;
        DWORD streamFlags = 0;
        LONGLONG timestamp = 0;

        hr = pSourceReader->ReadSample(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            0,
            &actualStreamIndex,
            &streamFlags,
            &timestamp,
            &pSample
        );


        if (FAILED(hr))
        { 
            Log("ReadSample FAILED: %s", HrToString(hr));
            break; 
        }
        if (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
        { 
            //Log("End of stream reached.");
            break; 
        }
        if (!pSample)
        { 
            Log("pSample is null but not end of stream.");
            break; 
        }


        Microsoft::WRL::ComPtr<IMFMediaBuffer> pBuffer;
        hr = pSample->ConvertToContiguousBuffer(&pBuffer);
        if (FAILED(hr)) { Log("ConvertToContiguousBuffer FAILED: %s", HrToString(hr)); break; }



        BYTE* pAudioData = nullptr;
        hr = pBuffer->Lock(&pAudioData, nullptr, &currentBufferLength);
        if (FAILED(hr)) { Log("Buffer Lock FAILED: %s", HrToString(hr)); break; }

        if (currentBufferLength > 0)
        {
            // audioData の末尾にデータを追加
            size_t cur = entry.audioData.size();
            entry.audioData.resize(cur + currentBufferLength);
            memcpy(entry.audioData.data() + cur, pAudioData, currentBufferLength);
            totalAudioDataSize += currentBufferLength;
            //Log("Read chunk: %u bytes. Total: %u bytes.", currentBufferLength, totalAudioDataSize);
        }

        hr = pBuffer->Unlock();
        if (FAILED(hr)) { Log("Buffer Unlock FAILED: %s", HrToString(hr)); break; }
    }


    //Log("Actual total audio data loaded: %u", totalAudioDataSize);
    //Log("Final audioData.size(): %u", (uint32_t)entry.audioData.size());

    if (hr != S_OK && hr != MF_E_END_OF_STREAM)
    {
        Log("LoadAudioの失敗 HRESULT: 0x%X", hr);
        if (entry.pWfx) { CoTaskMemFree(entry.pWfx); entry.pWfx = nullptr; entry.wfxSize = 0; }
        return UINT32_MAX;
    }

    // 読み込み完了後にメモリを安定化してから pAudioData を設定
    entry.audioData.shrink_to_fit();
    entry.audioBytes = totalAudioDataSize;

    // XAudio2Bufferの設定
    ZeroMemory(&entry.xAudioBuffer, sizeof(entry.xAudioBuffer));
    entry.xAudioBuffer.AudioBytes = entry.audioBytes;
    entry.xAudioBuffer.pAudioData = entry.audioData.data();
    entry.xAudioBuffer.LoopBegin = 0;
    entry.xAudioBuffer.LoopLength = 0;
    entry.xAudioBuffer.LoopCount = 0; // 0は1回再生

    /// ソースボイスの作成
    /// XAudio2Create が成功し、pXAudio2 が有効であることを確認
    if (!pXAudio2)
    {
        Log("XAudio2 engine not initialized when trying to create source voice.");
        if (entry.pWfx) { CoTaskMemFree(entry.pWfx); entry.pWfx = nullptr; entry.wfxSize = 0; }
        return UINT32_MAX;
    }

    //Log("--- Debugging WAVEFORMATEX for CreateSourceVoice ---");
    //Log("filePath: %s", filePath.c_str());
    //Log("wFormatTag: 0x%X (0x1 = WAVE_FORMAT_PCM)", entry.pWfx->wFormatTag);
    //Log("nChannels: %u", entry.pWfx->nChannels);
    //Log("nSamplesPerSec: %u Hz", entry.pWfx->nSamplesPerSec);
    //Log("nAvgBytesPerSec: %u bytes/sec", entry.pWfx->nAvgBytesPerSec);
    //Log("nBlockAlign: %u bytes", entry.pWfx->nBlockAlign);
    //Log("wBitsPerSample: %u bits", entry.pWfx->wBitsPerSample);
    //Log("cbSize: %u bytes (extra info size)", entry.pWfx->cbSize);
    //Log("--------------------------------------------------");


    hr = pXAudio2->CreateSourceVoice(&entry.pSourceVoice, entry.pWfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &voiceCallback);
    if (FAILED(hr))
    {
        Log("Failed to create source voice: 0x%X", hr);
        if (entry.pWfx) { CoTaskMemFree(entry.pWfx); entry.pWfx = nullptr; entry.wfxSize = 0; }
        return UINT32_MAX;
    }


    // マップに格納
    uint32_t id = nextAudioId++;
    loadedAudio[id] = std::move(entry);
    Log("成功 ID: %u", id);

    return id;
}

AudioData* AudioManager::GetAudioData(uint32_t audioId)
{
    auto it = loadedAudio.find(audioId);
	if (it != loadedAudio.end())
    {
        return &it->second;
    }
    else
    {
        Log("存在しないオーディオIDのデータを取得しました。ID: %u", audioId);
        assert(0);
        return nullptr;
    }
}

// 再生
void AudioManager::PlayAudio(const uint32_t& audioId, bool loop)
{
    auto it = loadedAudio.find(audioId);
    if (it != loadedAudio.end())
    {
        AudioData& entry = it->second;

        // すでに再生中であれば停止
        entry.pSourceVoice->Stop(0);
        entry.pSourceVoice->FlushSourceBuffers(); // バッファをクリア

        // loopが１の場合XAUDIO2_LOOP_INFINITE(無限ループ)に設定。0なら0
        entry.xAudioBuffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0; // ループ設定

        // オーディオデータをキューに送信し再生の準備をする(サブミット)
        HRESULT hr = entry.pSourceVoice->SubmitSourceBuffer(&entry.xAudioBuffer);
        if (FAILED(hr))
        {
            Log("Failed to submit source buffer for audio ID: %u HRESULT: 0x%X", audioId, hr);
            assert(0);
        }

        // 再生
        hr = entry.pSourceVoice->Start(0);
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

// 再生中のオーディオ停止
void AudioManager::StopAudio(const uint32_t& audioId)
{
    auto it = loadedAudio.find(audioId);
    if (it != loadedAudio.end())
    {
        it->second.pSourceVoice->Stop(0);
        it->second.pSourceVoice->FlushSourceBuffers();
        Log("%uのオーディオを停止しました", audioId);
    }
    else
    {
        Log("存在しないオーディオを停止しました。ID: %u", audioId);
        assert(0);
    }
}

// 特定のオーディオ音量のセッターゲッター
void AudioManager::SetVolume(const uint32_t& audioId, float volume)
{
    auto it = loadedAudio.find(audioId);
    // findできなかった場合はloadedAudio.end()がかえってくる。
    if (it != loadedAudio.end())
    {
        if (volume != GetVolume(audioId))
        {
            float clampedVolume = my_max(0.0f, my_min(1.0f, volume));
            it->second.pSourceVoice->SetVolume(clampedVolume);
            Log("%uのボリュームを%fに設定しました", audioId, clampedVolume);
        }

    }
    else
    {
        Log("存在しないオーディオのボリュームを設定しました。ID: %u", audioId);
        assert(0);
    }
}
float AudioManager::GetVolume(const uint32_t& audioId)
{
    auto it = loadedAudio.find(audioId);
    if (it != loadedAudio.end() && it->second.pSourceVoice)
    {
        float currentVolume = 0.0f;
        it->second.pSourceVoice->GetVolume(&currentVolume);
        return currentVolume;
    }
    Log("AudioID:%u が存在しないか意味分からんバグか", audioId);
    return 0.0f;
}

// マスター音量のセッターゲッター
void AudioManager::SetMasterVolume(float volume)
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
float AudioManager::GetMasterVolume()
{
    if (pMasteringVoice)
    {
        float currentVolume = 0.0f;
        pMasteringVoice->GetVolume(&currentVolume);
        return currentVolume;
    }
    Log("マスター音量を取得できませんでした。なんてログがでることは100%ありえません");
    return 0.0f;
}

// 現在再生してるか？
bool AudioManager::IsAudioPlaying(const uint32_t& audioId)
{
    auto it = loadedAudio.find(audioId);
    if (it != loadedAudio.end())
    {
        AudioData& entry = it->second;
        if (entry.pSourceVoice)
        {
            XAUDIO2_VOICE_STATE state;
            entry.pSourceVoice->GetState(&state);

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


// 解放のループ内でたくさん使う
void AudioManager::CleanupAudioData(AudioData& entry)
{
    if (entry.pSourceVoice)
    {
        entry.pSourceVoice->Stop(0);
        entry.pSourceVoice->FlushSourceBuffers();
        entry.pSourceVoice->DestroyVoice();
        entry.pSourceVoice = nullptr;
    }

    if (entry.pWfx)
    {
        CoTaskMemFree(entry.pWfx);
        entry.pWfx = nullptr;
        entry.wfxSize = 0;
    }

    entry.audioData.clear();
    entry.audioData.shrink_to_fit();
}


// なんかいずれ使えるらしいけどまだ理解できない・
void VoiceCallback::OnBufferEnd(void* pBufferContext)
{

}

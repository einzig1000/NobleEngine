#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

#include "AudioLoader.h"
#include <Utilities/Converter/StringConverter/StringConverter.h>
#include <Utilities/Logger/Logger.h>
#include <AssetManager/Audio/AudioBank/AudioBank.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>

AudioLoader::AudioLoader(AudioBank* bank)
	: bank_(bank)
{}

AudioLoader::~AudioLoader() {}

int32_t AudioLoader::LoadAudio(const std::string & filePath)
{
    Log("オーディオ読み込み開始 :%s", filePath.c_str());

    HRESULT hr = S_OK;
    std::unique_ptr<AudioData> data = std::make_unique<AudioData>();

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
    data->pWfx = wfx;
    data->wfxSize = formatSize;



    //Log("--- WAVEFORMATEX Debug Info ---");
    //Log("wFormatTag: 0x%X", data->pWfx->wFormatTag);
    //Log("nChannels: %u", data->pWfx->nChannels);
    //Log("nSamplesPerSec: %u", data->pWfx->nSamplesPerSec);
    //Log("nAvgBytesPerSec: %u", data->pWfx->nAvgBytesPerSec);
    //Log("nBlockAlign: %u", data->pWfx->nBlockAlign);
    //Log("wBitsPerSample: %u", data->pWfx->wBitsPerSample);
    //Log("cbSize: %u", data->pWfx->cbSize);
    //Log("-------------------------------");


    // オーディオデータの読み込み
    DWORD currentBufferLength = 0;
    DWORD totalAudioDataSize = 0; // 実際に読み込んだ総バイト数を追跡

    // 初期化時にいい感じの容量を予約 。必要に応じて拡張する
    data->audioData.reserve(1024 * 1024); // 1 MB を予約

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
            size_t cur = data->audioData.size();
            data->audioData.resize(cur + currentBufferLength);
            memcpy(data->audioData.data() + cur, pAudioData, currentBufferLength);
            totalAudioDataSize += currentBufferLength;
            //Log("Read chunk: %u bytes. Total: %u bytes.", currentBufferLength, totalAudioDataSize);
        }

        hr = pBuffer->Unlock();
        if (FAILED(hr)) { Log("Buffer Unlock FAILED: %s", HrToString(hr)); break; }
    }


    //Log("Actual total audio data loaded: %u", totalAudioDataSize);
    //Log("Final audioData.size(): %u", (uint32_t)data->audioData.size());

    if (hr != S_OK && hr != MF_E_END_OF_STREAM)
    {
        Log("LoadAudioの失敗 HRESULT: 0x%X", hr);
        if (data->pWfx) { CoTaskMemFree(data->pWfx); data->pWfx = nullptr; data->wfxSize = 0; }
        return UINT32_MAX;
    }

    // 読み込み完了後にメモリを安定化してから pAudioData を設定
    data->audioData.shrink_to_fit();
    data->audioBytes = totalAudioDataSize;

    // マップに格納
    int32_t id = bank_->AllocateAudioID();
    bank_->AddAudioData(filePath, id, std::move(data));
    Log("成功 ID: %u", id);

    return id;
}
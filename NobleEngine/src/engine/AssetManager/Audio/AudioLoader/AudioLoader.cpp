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
{
	defaultAudioID = LoadAudio("assets/engine/audio/SE/オーラ1.mp3");
}

AudioLoader::~AudioLoader() {}

int32_t AudioLoader::LoadAudio(const std::string & filePath)
{
    // すでに読み込まれていたらそのオーディオIDを返す
    int32_t audioID = bank_->IsAudioDataExist(filePath);
    if (audioID != -1) return audioID;

    Logger::Log("オーディオ読み込み開始 :%s", filePath.c_str());

    std::unique_ptr<AudioData> data = std::make_unique<AudioData>();

	// オーディオデータの読み込み
	if (!LoadAudioFile(filePath, data.get()))
	{
        Logger::Log("読み込み失敗しました。デフォルトオーディオが使用されます。");
		return defaultAudioID;
	}

    // マップに格納
    int32_t id = bank_->AllocateAudioID();
    bank_->AddAudioData(filePath, id, std::move(data));
    Logger::Log("成功 ID: %u", id);

    return id;
}

bool AudioLoader::LoadAudioFile(const std::string& filePath, AudioData* data)
{
    Microsoft::WRL::ComPtr<IMFSourceReader> pSourceReader;

    HRESULT hr = S_OK;

    // ソースリーダー(オーディオデータを読み取るためのインターフェース)の作成
    std::wstring wFilePath = StringConverter::Convert(filePath);
    hr = MFCreateSourceReaderFromURL(wFilePath.c_str(), nullptr, &pSourceReader);
    if (FAILED(hr))
    {
        Logger::Log("オーディオファイルを開けませんでした: %s", filePath); return false; 

    }

    // メディアファイルには 複数のストリーム（音声・動画・字幕など） が含まれていることがあるため音声を取得するよと設定しているらしい
    hr = pSourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
    if (FAILED(hr)) { Logger::Log("取得ストリームの設定に失敗: %s", Logger::HrToString(hr)); return false; }

    // Media Foundation に対して、オーディオストリームをPCM形式にデコードするように要求
    Microsoft::WRL::ComPtr<IMFMediaType> pOutputMediaType;
    hr = MFCreateMediaType(&pOutputMediaType);
    if (FAILED(hr)) { Logger::Log("PCM出力MFMediaTypeの作成に失敗: %s", Logger::HrToString(hr)); return false; }


    hr = pOutputMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    if (FAILED(hr)) { Logger::Log("PCM出力の主要タイプ設定に失敗: %s", Logger::HrToString(hr)); return false; }

    hr = pOutputMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    if (FAILED(hr)) { Logger::Log("PCM出力のサブタイプ設定に失敗: %s", Logger::HrToString(hr)); return false; }

    // 音声データがどんな形式(MP3,WAV,AACとか)で保存されているかを調べる
    hr = pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pOutputMediaType.Get());
    if (FAILED(hr)) { Logger::Log("出力タイプをPCMに設定できませんでした: %s", Logger::HrToString(hr)); return false; }

    // Media FoundationがPCMフォーマットに変えたはずなので確認
    Microsoft::WRL::ComPtr<IMFMediaType> pActualMediaType;
    hr = pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pActualMediaType);
    if (FAILED(hr)) { Logger::Log("実際のメディアタイプ取得に失敗: %s", Logger::HrToString(hr)); return false; }

    UINT32 formatSize = 0;
    WAVEFORMATEX* wfx = nullptr;
    hr = MFCreateWaveFormatExFromMFMediaType(pActualMediaType.Get(), &wfx, &formatSize, 0);
    if (FAILED(hr)) { Logger::Log("実際のメディアタイプのWAVEFORMATEX変換に失敗: %s", Logger::HrToString(hr)); return false; }
    data->pWfx = wfx;
    data->wfxSize = formatSize;



    //Logger::Log("--- WAVEFORMATEX Debug Info ---");
    //Logger::Log("wFormatTag: 0x%X", data->pWfx->wFormatTag);
    //Logger::Log("nChannels: %u", data->pWfx->nChannels);
    //Logger::Log("nSamplesPerSec: %u", data->pWfx->nSamplesPerSec);
    //Logger::Log("nAvgBytesPerSec: %u", data->pWfx->nAvgBytesPerSec);
    //Logger::Log("nBlockAlign: %u", data->pWfx->nBlockAlign);
    //Logger::Log("wBitsPerSample: %u", data->pWfx->wBitsPerSample);
    //Logger::Log("cbSize: %u", data->pWfx->cbSize);
    //Logger::Log("-------------------------------");


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
            Logger::Log("ReadSample FAILED: %s", Logger::HrToString(hr));
            break;
        }
        if (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            Logger::Log("End of stream reached.");
            break;
        }
        if (!pSample)
        {
            Logger::Log("pSample is null but not end of stream.");
            break;
        }


        Microsoft::WRL::ComPtr<IMFMediaBuffer> pBuffer;
        hr = pSample->ConvertToContiguousBuffer(&pBuffer);
        if (FAILED(hr)) { Logger::Log("ConvertToContiguousBuffer FAILED: %s", Logger::HrToString(hr)); break; }



        BYTE* pAudioData = nullptr;
        hr = pBuffer->Lock(&pAudioData, nullptr, &currentBufferLength);
        if (FAILED(hr)) { Logger::Log("Buffer Lock FAILED: %s", Logger::HrToString(hr)); break; }

        if (currentBufferLength > 0)
        {
            // audioData の末尾にデータを追加
            size_t cur = data->audioData.size();
            data->audioData.resize(cur + currentBufferLength);
            memcpy(data->audioData.data() + cur, pAudioData, currentBufferLength);
            totalAudioDataSize += currentBufferLength;
            //Logger::Log("Read chunk: %u bytes. Total: %u bytes.", currentBufferLength, totalAudioDataSize);
        }

        hr = pBuffer->Unlock();
        if (FAILED(hr)) { Logger::Log("Buffer Unlock FAILED: %s", Logger::HrToString(hr)); break; }
    }


    //Logger::Log("Actual total audio data loaded: %u", totalAudioDataSize);
    //Logger::Log("Final audioData.size(): %u", (uint32_t)data->audioData.size());

    if (hr != S_OK && hr != MF_E_END_OF_STREAM)
    {
        Logger::Log("LoadAudioの失敗 HRESULT: 0x%X", hr);
        if (data->pWfx) { CoTaskMemFree(data->pWfx); data->pWfx = nullptr; data->wfxSize = 0; }
        return false;
    }

    // 読み込み完了後にメモリを安定化してから pAudioData を設定
    data->audioData.shrink_to_fit();
    data->audioBytes = totalAudioDataSize;
    data->filePath = filePath;

    return true;
}

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
{
	bank_ = std::make_unique<AudioBank>();
	player_ = std::make_unique<AudioPlayer>(bank_.get());
	loader_ = std::make_unique<AudioLoader>(bank_.get(), player_->GetXAudio2());

    Log("コンストラクタ実行成功 : AudioManager");
}

// x
AudioManager::~AudioManager()
{
    Log("デストラクタ実行成功 : AudioManager");
}

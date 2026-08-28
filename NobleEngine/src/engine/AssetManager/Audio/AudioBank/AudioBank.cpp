#include "AudioBank.h"

AudioBank::AudioBank()
{}

AudioBank::~AudioBank()
{
    for (auto& pair : audioList_)
    {
        CleanupAudioData(*pair);
    }
    audioList_.clear();
}

void AudioBank::AddAudioData(const std::string& filePath, const int32_t ID, std::unique_ptr<AudioData> audioData)
{
	pathToIDMap_[filePath] = ID;
	audioList_.push_back(std::move(audioData));
}

int32_t AudioBank::IsAudioDataExist(const std::string & filePath)
{
	auto it = pathToIDMap_.find(filePath);
	if (it != pathToIDMap_.end())
	{
		return it->second;
	}
	return -1;
}

const AudioData* AudioBank::GetAudioData(int32_t audioID) const
{
	if (audioID >= 0 && audioID < static_cast<int32_t>(audioList_.size()))
	{
		return audioList_[audioID].get();
	}
	return nullptr;
}

void AudioBank::CleanupAudioData(AudioData& entry)
{
    if (entry.pWfx)
    {
        CoTaskMemFree(entry.pWfx);
        entry.pWfx = nullptr;
        entry.wfxSize = 0;
    }

    entry.audioData.clear();
    entry.audioData.shrink_to_fit();
}
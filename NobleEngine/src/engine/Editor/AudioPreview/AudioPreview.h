#pragma once
#include <EngineDefinition/EngineDefinition.h>

class AudioBank;
class AudioPlayer;

class AudioPreview
{
public:
	AudioPreview(AudioBank* bank, AudioPlayer* audioPlayer);
	~AudioPreview();

	void DrawImGui();

private:
	AudioBank* bank_;
	AudioPlayer* audioPlayer_;

	// 選択中の音を切り替える
	void ChangeSelectedAudio(int32_t audioID);

	const AudioData* audioData_ = nullptr;
	int32_t selectedAudioID_ = -1;
	int32_t playID_ = -1;
	bool loop_ = false;
	float volume_ = 1.0f;
};
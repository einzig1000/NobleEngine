#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <unordered_map>
#include <memory>

class AudioBank
{
public:
	AudioBank();
	~AudioBank();

	// オーディオデータを追加
	void AddAudioData(const std::string& filePath, const int32_t ID, std::unique_ptr<AudioData> audioData);
	// filePathが同じオーディオデータが存在するか
	int32_t IsAudioDataExist(const std::string& filePath);
	// 次のIDを取得
	int32_t AllocateAudioID() const { return static_cast<int32_t>(audioList_.size()); }
	// audioIDからオーディオデータを取得
	const AudioData* GetAudioData(int32_t audioID) const;
	// オーディオリストを取得
	const std::vector<std::unique_ptr<AudioData>>& GetAudioList() const { return audioList_; }

private:
	// キー：オーディオのファイルパス、値：ID
	std::unordered_map<std::string, int32_t> pathToIDMap_;
	// キー：ID、値：オーディオデータ
	std::vector<std::unique_ptr<AudioData>> audioList_;

	// オーディオの解放
	void CleanupAudioData(AudioData& entry);
};


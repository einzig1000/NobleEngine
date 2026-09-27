#include "AudioPreview.h"
#include <ImGuiManager/ImGuiManager.h>
#include <AssetManager/Audio/AudioBank/AudioBank.h>
#include <AssetManager/Audio/AudioPlayer/AudioPlayer.h>
#include <Utilities/FileDialog/FileDialog.h>
#include <Engine.h>
#include <Window/WindowManager.h>
#include <AssetManager/AssetManager.h>

AudioPreview::AudioPreview(AudioBank* bank, AudioPlayer* audioPlayer)
	: bank_(bank), audioPlayer_(audioPlayer)
{}

AudioPreview::~AudioPreview()
{}

void AudioPreview::DrawImGui()
{
	ImGui::Begin("Audio Preview");


	if (ImGui::Button("Open Audio"))
	{
		std::string path = FileDialog::OpenFile(
			Engine::Instance().GetWindowManager()->GetHwnd(),
			L"Open Audio",
			{ { L"Audio Files", L"*.wav;*.mp3;*.ogg;*.flac" }, { L"All Files", L"*.*" } }
		);

		if (!path.empty())
		{
			ChangeSelectedAudio(Engine::Instance().GetAssetManager()->GetAudioManager()->GetAudioLoader()->LoadAudio(path));
		}
	}

	if (ImGui::BeginListBox("##audio list"))
	{
		const std::vector<std::unique_ptr<AudioData>>& audioList = bank_->GetAudioList();
		for (int32_t i = 0; i < (int32_t)audioList.size(); ++i)
		{
			ImGui::PushID(i);
			if (ImGui::SelectableWithCopy(audioList[i]->filePath, selectedAudioID_ == i))
			{
				ChangeSelectedAudio(i);
			}
			ImGui::PopID();
		}
		ImGui::EndListBox();
	}

	if (selectedAudioID_ != -1)
	{
		ImGui::SameLine();
		Vector2 startCursorPos = Vector2{ ImGui::GetCursorPosX(), ImGui::GetCursorPosY() };
		float lineCount = 0.0f;

		ImGui::Text("%s", audioData_->filePath.c_str());
		if (audioData_->pWfx != nullptr && audioData_->pWfx->nAvgBytesPerSec > 0)
		{
			float seconds = static_cast<float>(audioData_->audioBytes) / static_cast<float>(audioData_->pWfx->nAvgBytesPerSec);
			lineCount += 1.0f;
			ImGui::SetCursorPosX(startCursorPos.x);
			ImGui::SetCursorPosY(startCursorPos.y + 22.0f * lineCount);
			ImGui::Text("%.2fs / %uch / %uHz", seconds, audioData_->pWfx->nChannels, audioData_->pWfx->nSamplesPerSec);
		}

		bool isPlaying = playID_ != -1 && audioPlayer_->IsAudioPlaying(playID_);

		lineCount += 1.0f;
		ImGui::SetCursorPosX(startCursorPos.x);
		ImGui::SetCursorPosY(startCursorPos.y + 22.0f * lineCount);
		ImGui::Checkbox("Loop", &loop_);

		lineCount += 1.0f;
		ImGui::SetCursorPosX(startCursorPos.x);
		ImGui::SetCursorPosY(startCursorPos.y + 22.0f * lineCount);
		ImGui::SetNextItemWidth(200.0f);
		if (ImGui::SliderFloat("Volume", &volume_, 0.0f, 1.0f) && isPlaying)
		{
			audioPlayer_->SetVolume(playID_, volume_);
		}

		ImGui::BeginDisabled(isPlaying);
		lineCount += 1.0f;
		ImGui::SetCursorPosX(startCursorPos.x);
		ImGui::SetCursorPosY(startCursorPos.y + 22.0f * lineCount);
		if (ImGui::Button("Play"))
		{
			playID_ = audioPlayer_->PlayAudio(selectedAudioID_, loop_, volume_);
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(!isPlaying);
		lineCount += 1.0f;
		ImGui::SetCursorPosX(startCursorPos.x);
		ImGui::SetCursorPosY(startCursorPos.y + 22.0f * lineCount);
		if (ImGui::Button("Stop"))
		{
			audioPlayer_->StopAudio(playID_);
		}
		ImGui::EndDisabled();
	}

	ImGui::End();
}

void AudioPreview::ChangeSelectedAudio(int32_t audioID)
{
	// 別の音に切り替えるときは再生中のものを止めておく(再生ボイスの放置を防ぐ)
	if (playID_ != -1 && audioPlayer_->IsAudioPlaying(playID_))
	{
		audioPlayer_->StopAudio(playID_);
	}
	selectedAudioID_ = audioID;
	audioData_ = bank_->GetAudioData(selectedAudioID_);
	playID_ = -1;
}

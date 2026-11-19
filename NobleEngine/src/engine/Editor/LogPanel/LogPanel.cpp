#include "LogPanel.h"
#include <Utilities/Logger/Logger.h>
#include <ImGuiManager/ImGuiManager.h>
#include <externals/MagicEnum/magic_enum.hpp>
#include <shellapi.h>
#include <format>

namespace
{
	// 重要度ごとの文字の色
	ImVec4 GetLevelColor(Logger::LogLevel level)
	{
		switch (level)
		{
		case Logger::LogLevel::Debug:   return ImVec4(0.60f, 0.60f, 0.60f, 1.0f);
		case Logger::LogLevel::Warning: return ImVec4(1.00f, 0.80f, 0.30f, 1.0f);
		case Logger::LogLevel::Error:   return ImVec4(1.00f, 0.40f, 0.40f, 1.0f);
		default:                        return ImGui::GetStyleColorVec4(ImGuiCol_Text);
		}
	}

	// 複数行ログの2行目以降の字下げ。"12:03:41.218 [Warning] " の幅
	constexpr int kContinuationIndent = 23;
}

void LogPanel::DrawImGui()
{
	PullNewEntries();

	const uint32_t warningCount = levelCounts_[magic_enum::enum_integer(Logger::LogLevel::Warning)];
	const uint32_t errorCount = levelCounts_[magic_enum::enum_integer(Logger::LogLevel::Error)];
	// "###" より後ろが窓の ID。件数が変わっても同じ窓として扱われる
	const std::string title = std::format("Log  (Warning {} / Error {})###LogPanel", warningCount, errorCount);

	if (!ImGui::Begin(title.c_str()))
	{
		ImGui::End();
		return;
	}

	// 1行目: 操作ボタン
	if (ImGui::Button("クリア")) Clear();
	ImGui::SameLine();
	if (ImGui::Button("コピー")) CopyVisibleLinesToClipboard();
	ImGui::SameLine();
	const bool hasLogFile = !Logger::GetLogFilePath().empty();
	ImGui::BeginDisabled(!hasLogFile);
	if (ImGui::Button("ファイルの場所を開く")) OpenLogFileLocation();
	ImGui::EndDisabled();
	ImGui::SameLine();
	ImGui::Checkbox("自動スクロール", &autoScroll_);

	// 2行目: 重要度で絞り込み
	bool isFirstLevel = true;
	for (Logger::LogLevel level : magic_enum::enum_values<Logger::LogLevel>())
	{
		const size_t index = magic_enum::enum_integer(level);
		if (!isFirstLevel) ImGui::SameLine();
		isFirstLevel = false;

		ImGui::PushStyleColor(ImGuiCol_Text, GetLevelColor(level));
		// "###" より後ろを ID にして、件数が変わってもチェックの状態が消えないようにする
		const std::string label = std::format("{} ({})###level{}", magic_enum::enum_name(level), levelCounts_[index], index);
		ImGui::Checkbox(label.c_str(), &showLevels_[index]);
		ImGui::PopStyleColor();
	}

	// 3行目: 文字で絞り込み
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputTextWithHint("##filter", "文字で絞り込み", &filterText_);

	ImGui::Separator();

	// 絞り込みを通る行だけ集める
	visibleLineIndices_.clear();
	for (size_t i = 0; i < lines_.size(); ++i)
	{
		if (PassesFilter(lines_[i])) visibleLineIndices_.push_back(i);
	}

	if (ImGui::BeginChild("##LogLines", ImVec2(0.0f, 0.0f), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
	{
		// 画面に見えている行だけ描く
		ImGuiListClipper clipper;
		clipper.Begin(static_cast<int>(visibleLineIndices_.size()));
		while (clipper.Step())
		{
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
			{
				const Line& line = lines_[visibleLineIndices_[static_cast<size_t>(row)]];

				ImGui::PushStyleColor(ImGuiCol_Text, GetLevelColor(line.level));
				if (line.isContinuation)
				{
					ImGui::Text("%*s%s", kContinuationIndent, "", line.text.c_str());
				}
				else
				{
					// enum_name は std::string_view なので、終わりの位置を長さで渡す
					const std::string_view levelName = magic_enum::enum_name(line.level);
					ImGui::Text("%s [%-7.*s] %s", line.time.c_str(), static_cast<int>(levelName.size()), levelName.data(), line.text.c_str());
				}
				ImGui::PopStyleColor();

				// マウスを乗せると呼び出し元を出す
				if (ImGui::IsItemHovered() && line.line != 0)
				{
					ImGui::SetTooltip("%s(%u)", line.file, line.line);
				}
			}
		}
		clipper.End();

		// 一番下を見ているときだけ、新しい行に合わせて下へ動かす（上を読んでいる間は動かさない）
		if (autoScroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		{
			ImGui::SetScrollHereY(1.0f);
		}
	}
	ImGui::EndChild();

	ImGui::End();
}

void LogPanel::PullNewEntries()
{
	pullBuffer_.clear();
	if (Logger::CopyHistorySince(lastSerial_, pullBuffer_) == 0) return;

	for (const Logger::LogEntry& entry : pullBuffer_)
	{
		lastSerial_ = entry.serial;
		levelCounts_[magic_enum::enum_integer(entry.level)]++;

		// 改行で分けて1行ずつ持つ
		size_t begin = 0;
		bool isFirst = true;
		while (true)
		{
			const size_t end = entry.message.find('\n', begin);
			Line line;
			line.level = entry.level;
			line.time = entry.time;
			line.text = entry.message.substr(begin, (end == std::string::npos) ? std::string::npos : end - begin);
			line.file = entry.file;
			line.line = entry.line;
			line.isContinuation = !isFirst;
			lines_.push_back(std::move(line));

			if (end == std::string::npos) break;
			begin = end + 1;
			isFirst = false;
		}
	}

	// 上限を超えたら古い方を捨てる（毎回1行ずつ消すと遅いので、まとめて消す）
	if (lines_.size() > kMaxLines)
	{
		const size_t removeCount = lines_.size() - kMaxLines + kMaxLines / 10;
		lines_.erase(lines_.begin(), lines_.begin() + static_cast<std::ptrdiff_t>(removeCount));
		// 件数は残っている行から数え直す（2行目以降は数えない）
		levelCounts_.fill(0);
		for (const Line& line : lines_)
		{
			if (!line.isContinuation) levelCounts_[magic_enum::enum_integer(line.level)]++;
		}
	}
}

void LogPanel::Clear()
{
	lines_.clear();
	levelCounts_.fill(0);
	// lastSerial_ はそのまま。消したログをもう一度受け取らないようにするため
}

bool LogPanel::PassesFilter(const Line& line) const
{
	if (!showLevels_[magic_enum::enum_integer(line.level)]) return false;
	if (filterText_.empty()) return true;
	return line.text.find(filterText_) != std::string::npos;
}

void LogPanel::CopyVisibleLinesToClipboard() const
{
	std::string text;
	for (size_t index : visibleLineIndices_)
	{
		const Line& line = lines_[index];
		if (line.isContinuation)
		{
			text += std::format("{:{}}{}\n", "", kContinuationIndent, line.text);
		}
		else
		{
			text += std::format("{} [{:<7}] {}\n", line.time, magic_enum::enum_name(line.level), line.text);
		}
	}
	ImGui::SetClipboardText(text.c_str());
}

void LogPanel::OpenLogFileLocation() const
{
	const std::wstring path = Logger::GetLogFilePath();
	if (path.empty()) return;

	// エクスプローラーでフォルダを開き、今回のファイルを選んだ状態にする
	const std::wstring arguments = L"/select,\"" + path + L"\"";
	ShellExecuteW(nullptr, L"open", L"explorer.exe", arguments.c_str(), nullptr, SW_SHOWNORMAL);
}
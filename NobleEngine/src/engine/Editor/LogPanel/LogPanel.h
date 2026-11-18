#pragma once
#include <Editor/IEditorPanel/IEditorPanel.h>
#include <Utilities/Logger/LogTypes.h>
#include <array>
#include <cstdint>
#include <string>
#include <vector>

/// <summary>
/// Logger の履歴を表示する ImGui の窓
/// </summary>
class LogPanel : public IEditorPanel
{
public:
	void DrawImGui() override;

private:
	// 窓が持つ行数の上限。超えたら古い方から捨てる
	static constexpr size_t kMaxLines = 10000;

	// 1行分の表示データ。複数行のログは行ごとに分ける（ImGuiListClipper は行の高さがそろっている前提のため）
	struct Line
	{
		Logger::LogLevel level = Logger::LogLevel::Debug;
		std::string time;
		std::string text;
		const char* file = "";
		uint32_t line = 0;
		bool isContinuation = false; // 複数行ログの2行目以降か
	};

	// Logger から新しいログを受け取る
	void PullNewEntries();
	// 表示中の行をすべて消す
	void Clear();
	// 絞り込みを通るか
	bool PassesFilter(const Line& line) const;
	// 絞り込み後の行をクリップボードへ
	void CopyVisibleLinesToClipboard() const;
	// エクスプローラーでログファイルの場所を開く
	void OpenLogFileLocation() const;

	std::vector<Line> lines_;
	std::vector<size_t> visibleLineIndices_;    // 毎フレーム作り直す（確保を使い回すためメンバに置く）
	std::vector<Logger::LogEntry> pullBuffer_;  // 同上
	uint64_t lastSerial_ = 0;

	std::array<uint32_t, Logger::kLogLevelCount> levelCounts_{};  // 重要度ごとの件数（行数ではなくログの件数）
	std::array<bool, Logger::kLogLevelCount> showLevels_ = { true, true, true, true };
	std::string filterText_;
	bool autoScroll_ = true;
};
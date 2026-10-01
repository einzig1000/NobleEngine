#pragma once
#include <EngineDefinition/EngineDefinition.h>

/// <summary>
/// 採掘ptの管理
/// </summary>
class MiningPointGauge
{
public:
	// ブロックを壊したとき。出す経験値のパワーを返す
	float AddPending(float point);
	// オーブが届いたとき
	void AddPoint(float point);
	// 使う
	bool Use(float cost);
	// レベルを使う
	bool UseLevel(int32_t level);

	void DrawImGui();

	float GetPoint() const { return point_; }
	float GetSumPoint() const { return level_ * levelUpPoint_ + point_; }
	int32_t GetLevel() const { return level_; }
	float GetGaugeRatio() const { return point_ / levelUpPoint_; }

private:
	// ブロックを壊すとBlockInfo::miningPointがAddPendingに送られる。
	// pending_にBlockInfo::miningPointが加算され、pointPerOrb_を超えた時経験値オーブが1粒出る。
	// オーブを取得した時AddPointに送られ、point_に加算される。


	float pending_ = 0.0f;			// 壊したブロックのptが溜まる。pointPerOrb_を超えたらオーブが出る
	float pointPerOrb_ = 1000.0f;	// オーブ1粒に必要なpt

	float point_ = 0;				// 所持ポイント
	int32_t level_ = 0;				// 現在のレベル
	float levelUpPoint_ = 10000.0f;	// レベルアップに必要なpt

};
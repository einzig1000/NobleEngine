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

	void DrawImGui();

	float GetPoint() const { return point_; }

private:
	// ブロックを壊すとBlockInfo::miningPointがAddPendingに送られる。
	// pending_にBlockInfo::miningPointが加算され、pointPerOrb_を超えた時経験値オーブが1粒出る。
	// オーブを取得した時AddPointに送られ、point_に加算される。


	float pending_ = 0.0f;			// 
	float pointPerOrb_ = 100.0f;	// オーブ1粒に必要なpt
	float point_ = 0;				// 所持ポイント

};


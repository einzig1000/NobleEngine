#include "MiningPointGauge.h"
#include <Game.h>



float MiningPointGauge::AddPending(float point)
{
	pending_ += point;

	if (pending_ >= pointPerOrb_)
	{
		float returnValue = pending_;
		pending_ = 0.0f;
		return returnValue;
	}
	return 0.0f;
}

void MiningPointGauge::AddPoint(float point)
{
	point_ += point;

	while (point_ >= levelUpPoint_)
	{
		point_ -= levelUpPoint_;
		level_++;
	}
}

bool MiningPointGauge::Use(float cost)
{
	float sum = level_ * levelUpPoint_ + point_;

	if (sum < cost) return false;

	sum -= cost;

	level_ = static_cast<int32_t>(sum / levelUpPoint_);
	point_ = sum - (level_ * levelUpPoint_);

	return true;
}

bool MiningPointGauge::UseLevel(int32_t level)
{
	if (level < 0 || level_ < level) return false;
	level_ -= level;
	return true;
}

void MiningPointGauge::DrawImGui()
{
	ImGui::Begin("MiningPointGauge");
	ImGui::Text("MiningPoint : %3f", point_);
	ImGui::Text("Pending : %3f / %3f", pending_, pointPerOrb_);
	ImGui::Text("Level : %d", level_);
	ImGui::Text("Gauge Ratio : %3f", GetGaugeRatio());
	ImGui::End();
}
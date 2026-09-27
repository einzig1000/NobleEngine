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
}

bool MiningPointGauge::Use(float cost)
{
	if (point_ < cost) return false;
	point_ -= cost;
	return true;
}

void MiningPointGauge::DrawImGui()
{
	ImGui::Begin("MiningPointGauge");
	ImGui::Text("MiningPoint : %3f", point_);
	ImGui::Text("Pending : %3f / %3f", pending_, pointPerOrb_);
	ImGui::End();
}
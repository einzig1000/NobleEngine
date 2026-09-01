#pragma once
#include <memory>
#include <TimeManager/FixFPS/FixFPS.h>
#include <TimeManager/TimeScaler/TimeScaler.h>

class TimeManager
{
public:
	TimeManager();
	void Update();

	FixFPS* GetFixFPS() const { return fixFPS_.get(); }
	TimeScaler* GetTimeScaler() const { return timeScaler_.get(); }
	float GetScaledDeltaTimeMs() const { return scaledDeltaTimeMs_; }
	void SetTimeScale(float scale) { timeScaler_->SetTimeScale(scale); }

private:
	std::unique_ptr<FixFPS> fixFPS_;
	std::unique_ptr<TimeScaler> timeScaler_;

	float scaledDeltaTimeMs_ = 0.0f;
};


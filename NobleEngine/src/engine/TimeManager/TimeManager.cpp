#include "TimeManager.h"

TimeManager::TimeManager()
{
	fixFPS_ = std::make_unique<FixFPS>();
	timeScaler_ = std::make_unique<TimeScaler>();
}

void TimeManager::Update()
{
	fixFPS_->Update();
	scaledDeltaTimeMs_ = fixFPS_->GetClampedDeltaTimeMs() * timeScaler_->GetTimeScale();
}

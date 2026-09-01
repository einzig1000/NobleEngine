#pragma once

class TimeScaler
{
public:
	TimeScaler();

	void SetTimeScale(float scale) { timeScale_ = scale; }
	float GetTimeScale() const { return timeScale_; }

private:
	float timeScale_ = 1.0f;
};


#pragma once
#include <GameObjects/Effect/Particle/ParticleManager.h>
#include <GameObjects/Effect/Trail/TrailManager.h>

class EventBus;

class EffectManager
{
public:
	EffectManager();

	void SetEventBus(EventBus* eventBus);

	void Update(int32_t c_ID);
	void Draw(int32_t rt_ID);

private:
	EventBus* eventBus_ = nullptr;
	void CheckExternalEvents();

	bool ableMoveAll_ = true;

	ParticleManager particleManager_;
	TrailManager trailManager_;
};


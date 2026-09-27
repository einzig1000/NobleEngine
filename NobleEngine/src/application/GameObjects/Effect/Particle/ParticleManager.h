#pragma once
#include <GameObjects/Effect/Particle/IParticle.h>
#include <GameObjects/Effect/Particle/GoTargetCurving/GoTargetCurving.h>

class ParticleManager
{
public:
	ParticleManager();
	void SetEventBus(EventBus* eventBus);

	void AddParticle(IParticle* particle);

	void Update(int32_t c_ID);
	void Draw(int32_t rt_ID);

	GoTargetCurving* GetGoTargetCurving() { return goTargetCurving_.get(); }

private:
	EventBus* eventBus_ = nullptr;
	bool ableMoveAll_ = true;

	std::unique_ptr<GoTargetCurving> goTargetCurving_;

};


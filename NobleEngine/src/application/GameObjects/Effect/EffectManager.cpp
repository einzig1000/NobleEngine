#include "EffectManager.h"
#include <System/EventBus/EventBus.h>

EffectManager::EffectManager()
{
}

void EffectManager::SetEventBus(EventBus* eventBus)
{
	particleManager_.SetEventBus(eventBus);
}

void EffectManager::Update(int32_t c_ID)
{
	particleManager_.Update(c_ID);
	trailManager_.Update(c_ID);
}

void EffectManager::Draw(int32_t rt_ID)
{
	particleManager_.Draw(rt_ID);
	trailManager_.Draw(rt_ID);
}


void EffectManager::CheckExternalEvents()
{

}
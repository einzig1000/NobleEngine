#include "IPhase.h"

void IPhase::ChangePhase(PHASE phase)
{
	nextPhase_ = phase;
}

PHASE IPhase::GetNextPhase()
{
	return nextPhase_;
}

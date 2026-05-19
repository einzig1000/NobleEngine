#include "PhaseParent.h"

void PhaseParent::ChangePhase(PHASE phase)
{
	nextPhase_ = phase;
}

PHASE PhaseParent::GetNextPhase()
{
	return nextPhase_;
}

#include "IPhase.h"

void IPhase::ChangePhase(Phase phase)
{
	nextPhase_ = phase;
}

Phase IPhase::GetNextPhase()
{
	return nextPhase_;
}

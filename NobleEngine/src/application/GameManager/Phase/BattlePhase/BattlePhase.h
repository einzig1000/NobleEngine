#pragma once
#include "GameManager/Phase/PhaseParent/PhaseParent.h"


class BattlePhase
	: public PhaseParent
{
public:
	BattlePhase();
	~BattlePhase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(PHASE phase) override { nextPhase_ = phase; }


private:

};


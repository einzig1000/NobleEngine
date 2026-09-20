#pragma once
#include <GameManager/Phase/IPhase.h>


class BattlePhase
	: public IPhase
{
public:
	BattlePhase();
	~BattlePhase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(Phase phase) override { nextPhase_ = phase; }


private:

};


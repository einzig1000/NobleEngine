#pragma once
#include <memory>
#include "Game.h"
#include "Phase/PhaseParent/PhaseParent.h"


class GameManager
{
public:
	GameManager();
	~GameManager();

	void Update();
	void Draw();
	void DrawImGui();


private:

	// フェーズクラス
	std::unique_ptr<PhaseParent> currentPhase_;

	std::unique_ptr<PhaseParent> CreatePhase(PHASE phase);

	PhaseContext phaseContext_;

};


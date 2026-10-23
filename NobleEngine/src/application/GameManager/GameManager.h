#pragma once
#include <memory>
#include <GameManager/Phase/IPhase.h>


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
	std::unique_ptr<IPhase> phaseInstance_;

	Phase currentPhase_ = Phase::Phase_None;
	Phase nextPhase_ = Phase::Phase_None;

	PhaseContext phaseContext_;


	std::unique_ptr<IPhase> CreatePhase(Phase phase);


};


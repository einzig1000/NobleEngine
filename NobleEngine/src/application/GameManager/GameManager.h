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
	std::unique_ptr<IPhase> currentPhase_;

	std::unique_ptr<IPhase> CreatePhase(Phase phase);

	PhaseContext phaseContext_;

};


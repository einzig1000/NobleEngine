#pragma once
#include <GameObjects/Character/MiningState/IMiningState.h>

class Player;

// スイング採掘：持っているアイテムを振った軌跡に沿ってブロックを破壊する
class SwingMining : public IMiningState
{
public:
	explicit SwingMining(Player* owner);

	void Update() override;
	MiningPattern GetPattern() const override { return MiningPattern::Swing; }

private:
	Player* owner_ = nullptr;
};

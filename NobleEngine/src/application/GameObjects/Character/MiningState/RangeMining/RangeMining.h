#pragma once
#include <Game.h>
#include <GameObjects/Character/MiningState/IMiningState.h>

class Player;

// 範囲採掘：始点・終点の2ブロックをクリックで指定し範囲内を一撃破壊する
class RangeMining : public IMiningState
{
public:
	explicit RangeMining(Player* owner);

	// 切り替わるたびに選択途中の状態を破棄する(前に選んだ1点目を持ち越さない)
	void Enter() override { state_ = State::WaitingForStart; }
	void Update() override;
	void DrawImGui() override;
	MiningPattern GetPattern() const override { return MiningPattern::Range; }

private:
	enum class State
	{
		WaitingForStart,
		WaitingForEnd,
	};

	Player* owner_ = nullptr;
	State state_ = State::WaitingForStart;
	Vector3int startChunk_;
	Vector3int startLocal_;
};

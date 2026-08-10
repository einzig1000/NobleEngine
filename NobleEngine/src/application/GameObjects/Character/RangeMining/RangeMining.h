#pragma once
#include <Game.h>

class Player;

// 範囲採掘：始点・終点の2ブロックをクリックで指定し、範囲内を一括破壊する
class RangeMining
{
public:
	explicit RangeMining(Player* owner);
	void Update();

	// モード切り替え時などに選択途中の状態を破棄する
	void Reset() { state_ = State::WaitingForStart; }
	// 1点目を選択済みかどうか(デバッグ表示用)
	bool HasStartPoint() const { return state_ == State::WaitingForEnd; }

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

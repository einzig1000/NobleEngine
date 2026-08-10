#pragma once

class Player;

// スイング採掘：持っているアイテムを振った軌跡(OBB)に沿ってブロックを破壊する
class SwingMining
{
public:
	explicit SwingMining(Player* owner);
	void Update();

private:
	Player* owner_ = nullptr;
};

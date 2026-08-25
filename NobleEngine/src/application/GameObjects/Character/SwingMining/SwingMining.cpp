#include "SwingMining.h"
#include <GameObjects/Character/Player/Player.h>

SwingMining::SwingMining(Player* owner)
	: owner_(owner)
{}

void SwingMining::Update()
{
	if (!Game::IO::Mouse::IsHeld(0)) return;

	const std::vector<OBB>& itemOBBs = owner_->GetHaveItemOBB();
	for (const auto& obb : itemOBBs)
	{
		owner_->DestroyBlockInOBB(obb);
	}
}

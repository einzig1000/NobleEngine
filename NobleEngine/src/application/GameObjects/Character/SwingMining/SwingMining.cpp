#include "SwingMining.h"
#include <GameObjects/Character/Player/Player.h>

SwingMining::SwingMining(Player* owner)
	: owner_(owner)
{}

void SwingMining::Update()
{
	//if (!Game::IO::Mouse::IsHeld(0)) return;



	const ColliderShape& collider = owner_->GetHaveItemWorldCollider();
	for (const auto& sphere : collider.spheres)
	{
		owner_->DestroyBlockInSphere(sphere);
	}
}

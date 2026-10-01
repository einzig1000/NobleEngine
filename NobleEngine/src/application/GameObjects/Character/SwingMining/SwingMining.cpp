#include "SwingMining.h"
#include <GameObjects/Character/Player/Player.h>
#include <App.h>

SwingMining::SwingMining(Player* owner)
	: owner_(owner)
{}

void SwingMining::Update()
{
	//if (!Game::IO::Mouse::IsHeld(0)) return;



	const ColliderShape& collider = owner_->GetHaveItemWorldCollider();
	ItemID itemID = owner_->GetHaveItem();
	const ItemInfo* itemInfo = App::Data::Item::Get(itemID);
	if (!itemInfo) return;
	const ToolInfo* toolInfo = App::Data::Item::Get(itemInfo->toolID);
	if (!toolInfo) return;
	for (const auto& sphere : collider.spheres)
	{
		owner_->DestroyBlockInSphere(sphere, toolInfo->miningPower + 100.0f);
	}
}

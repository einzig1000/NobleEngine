#include "Item/DropItem/DropItemManager.h"

DropItemManager::DropItemManager(){}

DropItemManager::~DropItemManager()
{}

void DropItemManager::AddItem(ItemID id, Vector3 pos)
{
	auto newItem = std::make_unique<DropItem>();
	newItem->SetMapManager(mapManager_);
	newItem->SetPlayer(player_);
	newItem->SetItem(id, pos);


	// 空きスロットを探す
	for (size_t i = 0; i < itemActiveFlags_.size(); ++i)
	{
		if (!itemActiveFlags_[i])
		{
			items_[i] = std::move(newItem);
			itemActiveFlags_[i] = true;
			return;
		}
	}

	// 空きがなかったので新規追加
	items_.push_back(std::move(newItem));
	itemActiveFlags_.push_back(true);

}

void DropItemManager::Update()
{
	for (size_t i = 0; i < items_.size(); ++i)
	{
		if (itemActiveFlags_[i] && items_[i])
		{
			items_[i]->Update();

			if (items_[i]->IsDestroy())
			{
				items_[i].reset();
				itemActiveFlags_[i] = false;
			}
		}
	}

}

void DropItemManager::Draw()
{
	for (size_t i = 0; i < items_.size(); ++i)
	{
		if (itemActiveFlags_[i] && items_[i])
		{
			items_[i]->Draw();
		}
	}

}


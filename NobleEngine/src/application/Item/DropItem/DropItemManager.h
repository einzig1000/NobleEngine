#pragma once
#include "definition/definition.h"
#include "Item/DropItem/DropItem.h"
#include <vector>
#include <memory>

class Player;
class MapManager;

class DropItemManager
{
public:
	DropItemManager();
	~DropItemManager();

	void SetMapManager(MapManager* mapManager) { mapManager_ = mapManager; }
	void SetPlayer(Player* player) { player_ = player; }
	void AddItem(ItemID id, Vector3 pos);
	void Update();
	void Draw();

	// ドロップアイテム配列
	std::vector<std::unique_ptr<DropItem>> items_;


private:
	std::vector<bool> itemActiveFlags_;

	Player* player_;

	MapManager* mapManager_;

};

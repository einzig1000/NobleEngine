#include "ItemDataManager.h"

ItemDataManager::ItemDataManager()
{
	bank_ = std::make_unique<ItemDataBank>();

	loader_ = std::make_unique<ItemDataLoader>(bank_.get());
}

ItemDataManager::~ItemDataManager()
{}


#include "ItemDataManager.h"

ItemDataManager::ItemDataManager()
{
	bank_ = std::make_unique<ItemDataBank>();

	loader_ = std::make_unique<ItemDataLoader>(bank_.get());

#ifdef _DEBUG
	editor_ = std::make_unique<ItemDataEditor>(bank_.get());
#endif
	
}

ItemDataManager::~ItemDataManager()
{}


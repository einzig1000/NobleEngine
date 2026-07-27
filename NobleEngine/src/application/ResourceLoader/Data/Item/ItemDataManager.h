#pragma once
#include "Bank/ItemDataBank.h"
#include "Loader/ItemDataLoader.h"
#include <memory>

class ItemDataManager
{
public:
	ItemDataManager();
	~ItemDataManager();

	ItemDataBank* getBank() const { return bank_.get(); }
	ItemDataLoader* getLoader() const { return loader_.get(); }

private:
	std::unique_ptr<ItemDataBank> bank_;
	std::unique_ptr<ItemDataLoader> loader_;
};


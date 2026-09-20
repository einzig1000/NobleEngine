#pragma once
#include "Bank/ItemDataBank.h"
#include "Loader/ItemDataLoader.h"
#include "Saver/ItemDataSaver.h"
#include <memory>

class ItemDataManager
{
public:
	ItemDataManager();
	~ItemDataManager();

	ItemDataBank* GetBank() const { return bank_.get(); }
	ItemDataLoader* GetLoader() const { return loader_.get(); }
	ItemDataSaver* GetSaver() const { return saver_.get(); }

private:
	std::unique_ptr<ItemDataBank> bank_;
	std::unique_ptr<ItemDataLoader> loader_;
	std::unique_ptr<ItemDataSaver> saver_;
};


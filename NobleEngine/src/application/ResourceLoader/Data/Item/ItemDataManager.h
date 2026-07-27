#pragma once
#include "Bank/ItemDataBank.h"
#include "Loader/ItemDataLoader.h"
#include "Editor/ItemDataEditor.h"
#include <memory>

class ItemDataManager
{
public:
	ItemDataManager();
	~ItemDataManager();

	ItemDataBank* getBank() const { return bank_.get(); }
	ItemDataLoader* getLoader() const { return loader_.get(); }
	ItemDataEditor* getEditor() const { return editor_.get(); }

private:
	std::unique_ptr<ItemDataBank> bank_;
	std::unique_ptr<ItemDataLoader> loader_;
	std::unique_ptr<ItemDataEditor> editor_;
};


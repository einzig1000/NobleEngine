#pragma once
#include "Bank/UIDataBank.h"
#include "Loader/UIDataLoader.h"
#include "Saver/UIDataSaver.h"
#include <memory>

class UIDataManager
{
public:
	UIDataManager();
	~UIDataManager();

	UIDataBank* GetBank() const { return bank_.get(); }
	UIDataLoader* GetLoader() const { return loader_.get(); }
	UIDataSaver* GetSaver() const { return saver_.get(); }

private:
	std::unique_ptr<UIDataBank> bank_;
	std::unique_ptr<UIDataLoader> loader_;
	std::unique_ptr<UIDataSaver> saver_;
};



#include "UIDataManager.h"

UIDataManager::UIDataManager()
{
	bank_ = std::make_unique<UIDataBank>();

	loader_ = std::make_unique<UIDataLoader>(bank_.get());

	saver_ = std::make_unique<UIDataSaver>(bank_.get());
}

UIDataManager::~UIDataManager()
{}
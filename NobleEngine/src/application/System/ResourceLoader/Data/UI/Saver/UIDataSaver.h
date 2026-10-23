#pragma once
#include <GameObjects/UI/UIData.h>

class UIDataBank;

class UIDataSaver
{
public:
	UIDataSaver(UIDataBank* bank);
	~UIDataSaver();

	void Save(UIElementType type, const std::vector<ElementData>& data);

private:
	UIDataBank* bank_;
};


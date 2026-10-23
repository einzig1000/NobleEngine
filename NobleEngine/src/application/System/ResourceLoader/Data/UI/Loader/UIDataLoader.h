#pragma once
#include <GameObjects/UI/UIData.h>

class UIDataBank;

class UIDataLoader
{
public:
	UIDataLoader(UIDataBank* bank);
	~UIDataLoader();

	void Load();
	void Load(UIElementType type);

private:
	UIDataBank* bank_;

};


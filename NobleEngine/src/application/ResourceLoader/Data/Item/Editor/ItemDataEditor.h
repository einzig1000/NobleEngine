#pragma once

class ItemDataBank;

class ItemDataEditor
{
public:
	ItemDataEditor(ItemDataBank* bank);
	~ItemDataEditor();

private:
	ItemDataBank* bank_;
};


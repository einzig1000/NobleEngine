#pragma once
#include "Game.h"

class BlockConfig
{
public:
	BlockConfig();
	~BlockConfig();

	Blockinfo GetBlockInfo(BlockID id) const;

private:
	std::map<BlockID, Blockinfo> blockInfoMap_;

};


#pragma once
#include <Game.h>
#include <definition/definition.h>

class BlockConfig
{
public:
	BlockConfig();
	~BlockConfig();

	BlockInfo GetBlockInfo(BlockID id) const;

private:
	std::map<BlockID, BlockInfo> blockInfoMap_;
	std::vector<Vector4uint> blockInfoTable_;
};


#include "definition.h"


ItemID BlockIDtoItemID(BlockID id)
{
    switch (id)
    {
	case BlockID::Air:      return ItemID::Block_Air; break;
    case BlockID::Stone:    return ItemID::Block_Stone; break;
	case BlockID::Iron:     return ItemID::Block_Iron; break;
	case BlockID::Gold:     return ItemID::Block_Gold; break;
	case BlockID::Diamond:  return ItemID::Block_Diamond; break;
	case BlockID::Bedrock:  return ItemID::Block_Bedrock; break;
	case BlockID::Glass:    return ItemID::Block_Glass; break;
	case BlockID::Dirt:     return ItemID::Block_Dirt; break;
	case BlockID::Grass:    return ItemID::Block_Grass; break;
	case BlockID::wood:     return ItemID::Block_Wood; break;
	case BlockID::Leaf:     return ItemID::Block_Leaf; break;
    }
	
	return ItemID::MAX;
}

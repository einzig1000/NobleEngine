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
	case BlockID::Wood:     return ItemID::Block_Wood; break;
	case BlockID::Leaf:     return ItemID::Block_Leaf; break;
    }
	
	return ItemID::MAX;
}

ItemID ToolIDtoItemID(ToolID id)
{
	switch (id)
	{
	case ToolID::Sword_of_Wood:	return ItemID::Tool_Sword_of_Wood; break;
	case ToolID::Sword_of_Stone:	return ItemID::Tool_Sword_of_Stone; break;
	case ToolID::Sword_of_Iron:	return ItemID::Tool_Sword_of_Iron; break;
	case ToolID::Sword_of_Gold:	return ItemID::Tool_Sword_of_Gold; break;
	case ToolID::Sword_of_Diamond:	return ItemID::Tool_Sword_of_Diamond; break;
	case ToolID::Pickel_of_Wood:	return ItemID::Tool_Pickel_of_Wood; break;
	case ToolID::Pickel_of_Stone:	return ItemID::Tool_Pickel_of_Stone; break;
	case ToolID::Pickel_of_Iron:	return ItemID::Tool_Pickel_of_Iron; break;
	case ToolID::Pickel_of_Gold:	return ItemID::Tool_Pickel_of_Gold; break;
	case ToolID::Pickel_of_Diamond:	return ItemID::Tool_Pickel_of_Diamond; break;
	case ToolID::Axe_of_Wood:	return ItemID::Tool_Axe_of_Wood; break;
	case ToolID::Axe_of_Stone:	return ItemID::Tool_Axe_of_Stone; break;
	case ToolID::Axe_of_Iron:	return ItemID::Tool_Axe_of_Iron; break;
	case ToolID::Axe_of_Gold:	return ItemID::Tool_Axe_of_Gold; break;
	case ToolID::Axe_of_Diamond:	return ItemID::Tool_Axe_of_Diamond; break;
	}

	return ItemID::MAX;
}
